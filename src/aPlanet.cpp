#include "layout/aPlanet.hpp"
#include "types/Achievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/SE_Sputnik.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/aEFilm.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPirate.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/aWarrior.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Planet.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/aTranclucator.hpp"
#include "units/aTransport.hpp"
#include "units/fEquipmentShop.hpp"

namespace aPlanet {
    // Nested in TryDispatchPirateAttacks; unused static link is caller-popped. Scores up to ten nearby stars and maps the score to 0.8..1.2.
    float CalculatePirateAttackNeighborhoodFactor(aGalaxy::TStar* Star);

    // Current ship must be a TPirate owned by the clan, in normal space, without an absolute order, absolute script order, script binding or partner. Ordinary nonabsolute orders are allowed.
    std::uint8_t IsShipEligible(aShip::TShip*& Ship);

    // Compared against an inclusive 0..100 roll for each economic event.
    std::int32_t EconomicEventChance = 4;

    // Five 32-byte rows, eight Integer weights in TKlingType order; SpawnWeightedDominatorShip selects row 1..5 from faction control.
    aPlanet::TDominatorSpawnWeightTable DominatorSpawnWeights = aPlanet::TDominatorSpawnWeightTable{{aPlanet::TDominatorSpawnWeightRow{{0, 50, 50, 100, 200, 600, 0, 0}}, aPlanet::TDominatorSpawnWeightRow{{0, 50, 50, 150, 250, 500, 2, 0}}, aPlanet::TDominatorSpawnWeightRow{{0, 50, 100, 200, 250, 400, 10, 0}}, aPlanet::TDominatorSpawnWeightRow{{0, 100, 100, 250, 250, 300, 20, 0}}, aPlanet::TDominatorSpawnWeightRow{{0, 100, 150, 300, 250, 200, 30, 0}}}};

    // Script.PlanetPirateClan; borrowed reference, retained after PirateWin(3).
    aPlanet::TPlanet* MainPiratePlanet{};

    void TSputnik_Create(TSputnik* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextSputnikId;
            ++aGalaxy::Galaxy->NextSputnikId;
        }
    }

    void TSputnik_Destroy(TSputnik* Self) {
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TSputnik::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        Buffer->AddDWord(Id);
        Buffer->AddWideStringZ(Graphic->GraphKey);
        EC_Buf::TBufEC* State = Graphic->BuildStateBuffer();
        Buffer->AddBuffer(State);
        pas::free(State);
        Buffer->AddSingle(Graphic->OrbitAngle);
    }

    void TSputnik::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        Id = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (Galaxy->NextSputnikId <= Id) {
            Galaxy->NextSputnikId = Id + 1;
        }
        {
            SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, Buffer->ReadWideString(), ClassesImports::Point(0, 0));
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, cpp_arg);
        }
        EC_Buf::TBufEC* State = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->ReadLengthPrefixedBuffer(State);
        Graphic->LoadStateBuffer(State);
        pas::free(State);
        Graphic->OrbitAngle = EC_Buf::TBufEC_GetSingle(Buffer);
    }

    void TPlanet_Create(TPlanet* Self) {
        EC_Struct::TObjectEx_Create(Self);
        if (aGalaxy::Galaxy != nullptr) {
            Self->Id = aGalaxy::Galaxy->NextPlanetId;
            ++aGalaxy::Galaxy->NextPlanetId;
            Self->GenerationSeed = aMyFunction::NextRandomIntRange(100000, SystemImports::MaxInt, aGalaxy::Galaxy->RandomState);
        }
        Self->RandomState = Self->GenerationSeed;
        Self->Warriors = pas::make_object<aMyFunction::TObjectList>();
        Self->RangerRelations = pas::make_object<aMyFunction::TObjectList>();
        Self->EquipmentShop = pas::make_object<aMyFunction::TObjectList>();
        Self->Satellites = pas::make_object<aMyFunction::TObjectList>();
        Self->Graphic = nullptr;
        Self->Reserved34 = 0u;
    }

    void TPlanet_Destroy(TPlanet* Self) {
        std::int32_t i{};
        PPlanetSurfaceLootEntry Entry{};
        pas::free(Self->Satellites);
        pas::list_clear(Self->Warriors);
        pas::free(Self->Warriors);
        Self->Warriors = nullptr;
        if (aKling::DominatorSpawnPlanet != Self) {
            i = pas::list_indexof(aGalaxy::Galaxy->Planets, reinterpret_cast<void*>(Self));
            if (i >= 0) {
                pas::list_delete(aGalaxy::Galaxy->Planets, i);
            }
        }
        // Native code deletes entries but leaves the RangerRelations list allocated.
        {
            const std::int32_t cpp_first = pas::list_count(Self->RangerRelations) - 1;
            if (cpp_first >= 0) {
                for (i = cpp_first; i >= 0; --i) {
                    pas::list_delete(Self->RangerRelations, i);
                }
            }
        }
        if (Self->Graphic != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Self->Graphic));
        }
        pas::free(Self->EquipmentShop);
        Self->EquipmentShop = nullptr;
        if (Self->SurfaceLootEntries != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->SurfaceLootEntries) - 1); cpp_range.next(i); ) {
                Entry = pas::list_at<TPlanetSurfaceLootEntry>(Self->SurfaceLootEntries, i);
                pas::free(Entry->Item);
                Entry->Item = nullptr;
                pas::dispose(Entry);
            }
            pas::free(Self->SurfaceLootEntries);
            Self->SurfaceLootEntries = nullptr;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Fourth argument: 0 selects the Solar System, 1..3 limit inhabited planets, 10/11 select special systems. Caller inserts Self into the star's planet list.
    void TPlanet::InitGenerated(aGalaxy::TStar* Star, std::int32_t TotalPlanetCount, std::int32_t InhabitedCountOrSpecialMode) {
        TPlanet* OtherPlanet{};
        TPlanet* PreviousPlanet{};
        std::uint8_t Invention{};
        std::uint8_t Good{};
        std::int32_t Quantity{};
        std::int32_t Count{};
        std::int32_t I{};
        std::int32_t Part{};
        std::int32_t LeastOwnerPlanetCount{};
        std::int32_t ExistingRing{};
        std::int32_t ModuleIndex{};
        std::uint32_t SavedRandomState{};
        std::int32_t EconomyRoll{};
        double PreviousExtent{};
        double SatelliteRadius{};
        double MinOrbitRadius{};
        double MaxOrbitRadius{};
        aGalaxyStruct::TPlanetGovernment GovernmentCandidate{};
        aGalaxyStruct::TOwnerId ItemOwner{};
        aGalaxyStruct::TOwnerId OwnerLoop{};
        pas::WideString BlockName{};
        TSputnik* Satellite{};
        std::int32_t SatelliteCount{};
        EC_BlockPar::TBlockParEC* SatelliteConfig{};
        aItem::TEquipment* Item{};
        aConst::TItemType ItemType{};
        std::int32_t Series{};
        std::uint8_t GovernmentRoll{};
        std::uint8_t RingKind{};
        std::uint8_t TemplateAvailable{};
        std::uint8_t AllowRing{};
        std::uint8_t IsSolar{};
        std::uint8_t HullType{};
        aItem::TEquipmentWithActCode* Loot{};
        aItem::TMicroModule* Module{};
        aItem::TCistern* Cistern{};
        aItem::TGoods* GoodsItem{};
        std::int32_t Weight{};
        std::int32_t Level{};
        aConst::PWeaponInfo WeaponInfo{};
        float MinSizeFactor{};
        float MaxSizeFactor{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t WeaponTechLevel{};
        aItem::TWeapon* GeneratedWeapon{};
        CurrentStar = Star;
        if (InhabitedCountOrSpecialMode == 0) {
            IsSolar = true;
        } else {
            IsSolar = false;
        }
        if (IsSolar) {
            RaceId = aGalaxyStruct::oiHuman;
            Name = GR_Main::LanguageDataConfig->GetBlock(u"PlanetName"sv)->GetBlock(u"Solar"sv)->GetParamValue(pas::list_count(Star->Planets));
            {
                pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 1, RandomState) * 2 - 1;
                OrbitalVelocity = cpp_left * (4.5L - pas::real_divide(pas::list_count(Star->Planets), 2.0L));
            }
            Orbit.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
            SpriteTemplateIndex = Globals::FindPlanetSpaceTemplateIndex(1, pas::list_count(Star->Planets) + 1);
            GraphicRadius = Globals::PlanetSpaceTemplates[SpriteTemplateIndex].Radius;
            {
                SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Planet::TPlanetSE>(SE_Planet::TPlanetSE_Create);
                pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                SE_Space::RetainSpaceObject(graphic, cpp_arg);
            }
            Globals::PlanetSpaceTemplates[SpriteTemplateIndex].SpaceObject->CopyTo(Graphic);
            GraphName = Graphic->GraphKey;
            Radius = GraphicRadius;
            Graphic->SetPosition(aMyFunction::PolarToPoint(Orbit));
            {
                std::uint32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(60, 100, pas::list_count(Star->Planets) * 3 + 47);
                SE_Planet::TPlanetSE* graphic_2 = Graphic;
                graphic_2->SetRotationTimerInterval(seededRandomIntRange);
            }
            Graphic->SetSurfaceMapStep(-1);
            Graphic->OrbitalVelocity = OrbitalVelocity;
            switch (pas::list_count(Star->Planets)) {
                case 0: {
                    Orbit.Radius = Radius + Star->SystemRadius + 175;
                    OwnerId = aGalaxyStruct::oiUninhabited;
                    Government = aGalaxyStruct::pgAnarchy;
                    Economy = aGalaxyStruct::peAgricultural;
                    break;
                }
                case 1: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L;
                    OwnerId = aGalaxyStruct::oiHuman;
                    Government = aGalaxyStruct::pgDemocracy;
                    Economy = aGalaxyStruct::peAgricultural;
                    Population = CalculateBasePopulation();
                    break;
                }
                case 2: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L;
                    OwnerId = aGalaxyStruct::oiHuman;
                    Government = aGalaxyStruct::pgDemocracy;
                    Economy = aGalaxyStruct::peIndustrial;
                    Population = 1000000;
                    Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                    pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                    MinOrbitRadius = System::Round(Radius * 1.3L);
                    {
                        SE_Space::TObjectSE* cpp_arg_2 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, u"Sputnik.Moon"_wref.get(), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> graphic_3 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                        SE_Space::RetainSpaceObject(graphic_3, cpp_arg_2);
                    }
                    Satellite->Graphic->DepthOrder = 0;
                    Satellite->Graphic->OrbitCenter = GetPosition();
                    Satellite->Graphic->OrbitRadius = MinOrbitRadius;
                    Satellite->Graphic->OrbitAngle = 1.8E+2f;
                    Satellite->Graphic->OrbitAngleStep = 1.1f;
                    Satellite->Graphic->OrbitTimerInterval = 30u;
                    Satellite->Graphic->OrbitInclination = 7.0E+1f;
                    Satellite->Graphic->OrbitRotation = 2.5E+2f;
                    Satellite->Graphic->MinDisplayRadius = System::Round(GlobalsV::GeneratedSatelliteBaseRadius * 1.2L);
                    Satellite->Graphic->MaxDisplayRadius = System::Round(GlobalsV::GeneratedSatelliteBaseRadius * 1.5L);
                    Satellite->Graphic->RotationTimerInterval = 25u;
                    Satellite->Graphic->SurfaceMapStep = 1;
                    break;
                }
                case 3: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
                    OwnerId = aGalaxyStruct::oiHuman;
                    Government = aGalaxyStruct::pgDictatorship;
                    Economy = aGalaxyStruct::peIndustrial;
                    Population = 100000;
                    SatelliteCount = 2;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range.next(I); ) {
                        Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                        pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                        MinOrbitRadius = System::Round(Radius * 1.3L);
                        MaxOrbitRadius = Radius * 2;
                        {
                            SE_Space::TObjectSE* cpp_arg_3 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, static_cast<pas::WideString>(pas::concat_ansi({"Sputnik.Mars", SysUtils::IntToStr(I)})), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_4 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                            SE_Space::RetainSpaceObject(graphic_4, cpp_arg_3);
                        }
                        Satellite->Graphic->DepthOrder = I;
                        Satellite->Graphic->OrbitCenter = GetPosition();
                        Satellite->Graphic->OrbitRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                        Satellite->Graphic->OrbitAngle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
                        Satellite->Graphic->OrbitAngleStep = 1.0L + aMyFunction::NextRandomUnitFloat(RandomState);
                        Satellite->Graphic->OrbitTimerInterval = 30u;
                        Satellite->Graphic->OrbitInclination = aMyFunction::NextRandomIntRange(50, 120, RandomState);
                        Satellite->Graphic->OrbitRotation = aMyFunction::NextRandomIntRange(200, 350, RandomState);
                        Satellite->Graphic->MinDisplayRadius = GlobalsV::GeneratedSatelliteBaseRadius;
                        Satellite->Graphic->MaxDisplayRadius = System::Round(Satellite->Graphic->MinDisplayRadius * 1.5L);
                        Satellite->Graphic->RotationTimerInterval = 25u;
                        Satellite->Graphic->SurfaceMapStep = 1;
                    }
                    break;
                }
                case 4: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
                    OwnerId = aGalaxyStruct::oiUninhabited;
                    Government = aGalaxyStruct::pgAnarchy;
                    Economy = aGalaxyStruct::peAgricultural;
                    SatelliteCount = 4;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_2.next(I); ) {
                        Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                        pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                        MinOrbitRadius = System::Round(Radius * 1.3L);
                        MaxOrbitRadius = Radius * 2;
                        {
                            SE_Space::TObjectSE* cpp_arg_5 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, pas::concat_wide({u"Sputnik.", ([&] {
                                std::int32_t nextRandomIntRange = ([&] {
                                    std::int32_t cpp_arg_4 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get())->GetBlockCount() - 1;
                                    std::uint32_t& randomState = RandomState;
                                    return aMyFunction::NextRandomIntRange(0, cpp_arg_4, randomState);
                                }());
                                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get());
                                return blockByPath->GetBlockNameByIndex(nextRandomIntRange);
                            }())}), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_5 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                            SE_Space::RetainSpaceObject(graphic_5, cpp_arg_5);
                        }
                        Satellite->Graphic->DepthOrder = I;
                        Satellite->Graphic->OrbitCenter = GetPosition();
                        Satellite->Graphic->OrbitRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                        Satellite->Graphic->OrbitAngle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
                        Satellite->Graphic->OrbitAngleStep = 1.0L + aMyFunction::NextRandomUnitFloat(RandomState);
                        Satellite->Graphic->OrbitTimerInterval = aMyFunction::NextRandomIntRange(30, 35, RandomState);
                        Satellite->Graphic->OrbitInclination = aMyFunction::NextRandomIntRange(50, 120, RandomState);
                        Satellite->Graphic->OrbitRotation = aMyFunction::NextRandomIntRange(200, 350, RandomState);
                        Satellite->Graphic->MinDisplayRadius = GlobalsV::GeneratedSatelliteBaseRadius;
                        {
                            std::int32_t min = std::min<std::int32_t>(GlobalsV::MaximumSatelliteTemplateRadius, Satellite->Graphic->MinDisplayRadius * 2);
                            {
                                double cpp_arg_6 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), 1.0L);
                                double cpp_arg_7 = Satellite->Graphic->MinDisplayRadius * 1.3L;
                                Satellite->Graphic->MaxDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_6, 0.0, 1.0, cpp_arg_7, min));
                            }
                        }
                        Satellite->Graphic->RotationTimerInterval = 25u;
                        Satellite->Graphic->SurfaceMapStep = 1;
                    }
                    break;
                }
                case 5: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
                    OwnerId = aGalaxyStruct::oiUninhabited;
                    Government = aGalaxyStruct::pgRepublic;
                    Economy = aGalaxyStruct::peMixed;
                    Population = 120000;
                    Graphic->SetRingKind(22);
                    break;
                }
                case 6: {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
                    OwnerId = aGalaxyStruct::oiUninhabited;
                    Government = aGalaxyStruct::pgAnarchy;
                    Economy = aGalaxyStruct::peAgricultural;
                    SatelliteCount = 1;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_3.next(I); ) {
                        Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                        pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                        MinOrbitRadius = System::Round(Radius * 1.3L);
                        MaxOrbitRadius = Radius * 2;
                        {
                            SE_Space::TObjectSE* cpp_arg_9 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, pas::concat_wide({u"Sputnik.", ([&] {
                                std::int32_t nextRandomIntRange_2 = ([&] {
                                    std::int32_t cpp_arg_8 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get())->GetBlockCount() - 1;
                                    std::uint32_t& randomState_2 = RandomState;
                                    return aMyFunction::NextRandomIntRange(0, cpp_arg_8, randomState_2);
                                }());
                                EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get());
                                return blockByPath_2->GetBlockNameByIndex(nextRandomIntRange_2);
                            }())}), ClassesImports::Point(0, 0));
                            pas::Var<SE_Space::TObjectSE*> graphic_6 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                            SE_Space::RetainSpaceObject(graphic_6, cpp_arg_9);
                        }
                        Satellite->Graphic->DepthOrder = I;
                        Satellite->Graphic->OrbitCenter = GetPosition();
                        Satellite->Graphic->OrbitRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                        Satellite->Graphic->OrbitAngle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
                        Satellite->Graphic->OrbitAngleStep = 1.0L + aMyFunction::NextRandomUnitFloat(RandomState);
                        Satellite->Graphic->OrbitTimerInterval = 30u;
                        Satellite->Graphic->OrbitInclination = aMyFunction::NextRandomIntRange(50, 120, RandomState);
                        Satellite->Graphic->OrbitRotation = aMyFunction::NextRandomIntRange(1, 359, RandomState);
                        Satellite->Graphic->MinDisplayRadius = GlobalsV::GeneratedSatelliteBaseRadius;
                        {
                            double cpp_arg_10 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), 1.0L);
                            double cpp_arg_11 = pas::real_divide(GlobalsV::MaximumSatelliteTemplateRadius, 2.0L);
                            double maximumSatelliteTemplateRadius = GlobalsV::MaximumSatelliteTemplateRadius;
                            Satellite->Graphic->MaxDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_10, 0.0, 1.0, cpp_arg_11, maximumSatelliteTemplateRadius));
                        }
                        Satellite->Graphic->RotationTimerInterval = 25u;
                        Satellite->Graphic->SurfaceMapStep = 1;
                    }
                    break;
                }
            }
        } else {
            if (InhabitedCountOrSpecialMode > 3) {
                if (pas::list_count(Star->Planets) == 1 && InhabitedCountOrSpecialMode == 10) {
                    OwnerId = aGalaxyStruct::oiPirate;
                    RaceId = aGalaxyStruct::oiHuman;
                    IsMainPiratePlanet = true;
                    MainPiratePlanet = this;
                } else if (InhabitedCountOrSpecialMode == 11 && pas::list_count(Star->Planets) < 5) {
                    OwnerId = aGalaxyStruct::oiPirate;
                    RaceId = static_cast<aGalaxyStruct::TOwnerId>(pas::list_count(Star->Planets));
                } else {
                    OwnerId = aGalaxyStruct::oiUninhabited;
                }
            } else {
                Count = 0;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_4.next(I); ) {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, I);
                    if (PreviousPlanet->OwnerId != aGalaxyStruct::oiUninhabited) {
                        ++Count;
                    }
                }
                if (InhabitedCountOrSpecialMode == Count) {
                    OwnerId = aGalaxyStruct::oiUninhabited;
                } else if (TotalPlanetCount - pas::list_count(Star->Planets) <= InhabitedCountOrSpecialMode - Count || aMyFunction::NextRandomUnitFloat(RandomState) < 0.7L || pas::list_indexof(aGalaxy::Galaxy->Stars, reinterpret_cast<void*>(Star)) < 5 && pas::list_count(Star->Planets) == 0) {
                    switch (aGalaxy::Galaxy->FindConstellationIndexForStar(CurrentStar)) {
                        case 0:
                        case 6: {
                            OwnerId = aGalaxyStruct::oiMaloc;
                            RaceId = aGalaxyStruct::oiMaloc;
                            break;
                        }
                        case 1:
                        case 5: {
                            OwnerId = aGalaxyStruct::oiPeleng;
                            RaceId = aGalaxyStruct::oiPeleng;
                            break;
                        }
                        case 2:
                        case 7: {
                            OwnerId = aGalaxyStruct::oiHuman;
                            RaceId = aGalaxyStruct::oiHuman;
                            break;
                        }
                        case 3:
                        case 8: {
                            OwnerId = aGalaxyStruct::oiFeyan;
                            RaceId = aGalaxyStruct::oiFeyan;
                            break;
                        }
                        case 4:
                        case 9: {
                            OwnerId = aGalaxyStruct::oiGaal;
                            RaceId = aGalaxyStruct::oiGaal;
                            break;
                        }
                        default: {
                            switch (aMyFunction::NextRandomIntRange(0, 4, RandomState)) {
                                case 0: {
                                    OwnerId = aGalaxyStruct::oiMaloc;
                                    RaceId = aGalaxyStruct::oiMaloc;
                                    break;
                                }
                                case 1: {
                                    OwnerId = aGalaxyStruct::oiPeleng;
                                    RaceId = aGalaxyStruct::oiPeleng;
                                    break;
                                }
                                case 2: {
                                    OwnerId = aGalaxyStruct::oiHuman;
                                    RaceId = aGalaxyStruct::oiHuman;
                                    break;
                                }
                                case 3: {
                                    OwnerId = aGalaxyStruct::oiFeyan;
                                    RaceId = aGalaxyStruct::oiFeyan;
                                    break;
                                }
                                case 4: {
                                    OwnerId = aGalaxyStruct::oiGaal;
                                    RaceId = aGalaxyStruct::oiGaal;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    if ((Count > 0 || aGalaxy::Galaxy->FindConstellationIndexForStar(CurrentStar) > 4) && (aMyFunction::NextRandomUnitFloat(RandomState) < 0.2L || aGalaxy::Galaxy->FindConstellationIndexForStar(CurrentStar) > 9) && pas::list_indexof(aGalaxy::Galaxy->Stars, reinterpret_cast<void*>(Star)) > 4) {
                        ItemOwner = aGalaxyStruct::oiMaloc;
                        LeastOwnerPlanetCount = 10000;
                        for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range_5.next(OwnerLoop); ) {
                            if (CurrentStar->CountDistinctInhabitedPlanetOwners() == 2 && CurrentStar->CountPlanetsByOwner(OwnerLoop) == 0) {
                                continue;
                            }
                            Count = 0;
                            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range_6.next(I); ) {
                                PreviousPlanet = pas::list_at<TPlanet>(aGalaxy::Galaxy->Planets, I);
                                if (PreviousPlanet->OwnerId == OwnerLoop) {
                                    ++Count;
                                }
                            }
                            if (LeastOwnerPlanetCount > Count) {
                                LeastOwnerPlanetCount = Count;
                                ItemOwner = OwnerLoop;
                            }
                        }
                        OwnerId = ItemOwner;
                        RaceId = aConst::OwnerToRace(ItemOwner);
                    }
                } else {
                    OwnerId = aGalaxyStruct::oiUninhabited;
                }
            }
            BlockName = aConst::OwnerToSys(OwnerId);
            if (OwnerId != aGalaxyStruct::oiPirate) {
                Name = pas::WideString();
            } else {
                if (InhabitedCountOrSpecialMode == 10) {
                    Quantity = 0;
                } else {
                    Quantity = pas::list_count(Star->Planets) + 1;
                }
                Name = GR_Main::LanguageDataConfig->GetBlock(u"PlanetName"sv)->GetBlock(pas::view(BlockName))->GetParamValue(Quantity);
            }
            if (InhabitedCountOrSpecialMode == 10 && OwnerId == aGalaxyStruct::oiPirate) {
                {
                    pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
                    OrbitalVelocity = cpp_left_2 * (4.5L - pas::real_divide(pas::list_count(Star->Planets), 2.0L));
                }
                Orbit.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
                GraphicRadius = 80;
                {
                    SE_Space::TObjectSE* cpp_arg_12 = pas::construct_call<SE_Planet::TPlanetSE>(SE_Planet::TPlanetSE_CreateFromGraph, u"Ruins.RG"_wref.get(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic_7 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_7, cpp_arg_12);
                }
                GraphName = Graphic->GraphKey;
                Radius = GraphicRadius;
                Graphic->SetPosition(aMyFunction::PolarToPoint(Orbit));
                {
                    std::uint32_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(60, 100, RandomState);
                    SE_Planet::TPlanetSE* graphic_8 = Graphic;
                    graphic_8->SetRotationTimerInterval(nextRandomIntRange_3);
                }
                {
                    std::int32_t cpp_arg_13 = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
                    SE_Planet::TPlanetSE* graphic_9 = Graphic;
                    graphic_9->SetSurfaceMapStep(cpp_arg_13);
                }
                Graphic->OrbitalVelocity = OrbitalVelocity;
                PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L;
            } else {
                Count = Globals::PlanetSpaceTemplates.length() - 1 + 1;
                Part = 0;
                Quantity = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                while (true) {
                    TemplateAvailable = true;
                    ++Part;
                    aMyFunction::IncrementWrapped(Quantity, 0, Count - 1);
                    if (Globals::PlanetSpaceTemplates[Quantity].Style == 1) {
                        continue;
                    }
                    if (CountPlanetsOfSameRace() == 0) {
                        if (Globals::PlanetSpaceTemplates[Quantity].Radius > 80) {
                            continue;
                        }
                    } else if (CountPlanetsOfSameRace() == 1 && OwnerId != aGalaxyStruct::oiUninhabited) {
                        if (Globals::PlanetSpaceTemplates[Quantity].Radius < 100) {
                            continue;
                        }
                    } else if (pas::list_count(Star->Planets) > 0) {
                        PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                        if (pas::abs(Globals::PlanetSpaceTemplates[Quantity].Radius - PreviousPlanet->Radius) < 11) {
                            continue;
                        }
                    }
                    if (Part < Count) {
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range_7.next(I); ) {
                            OtherPlanet = pas::list_at<TPlanet>(aGalaxy::Galaxy->Planets, I);
                            if (OtherPlanet->SpriteTemplateIndex == Quantity && (OtherPlanet->CurrentStar == Star || aMyFunction::PointDistanceSquared(OtherPlanet->CurrentStar->Position, Star->Position) < 2.5E+3L)) {
                                TemplateAvailable = false;
                                break;
                            }
                        }
                    }
                    if (TemplateAvailable) {
                        break;
                    }
                }
                SpriteTemplateIndex = Quantity;
                GraphicRadius = Globals::PlanetSpaceTemplates[Quantity].Radius;
                {
                    SE_Space::TObjectSE* cpp_arg_14 = pas::construct_call<SE_Planet::TPlanetSE>(SE_Planet::TPlanetSE_Create);
                    pas::Var<SE_Space::TObjectSE*> graphic_10 = pas::Var<SE_Space::TObjectSE*>(&Graphic);
                    SE_Space::RetainSpaceObject(graphic_10, cpp_arg_14);
                }
                Globals::PlanetSpaceTemplates[Quantity].SpaceObject->CopyTo(Graphic);
                GraphName = Graphic->GraphKey;
                AllowRing = true;
                ExistingRing = 0;
                for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_8.next(I); ) {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, I);
                    if (PreviousPlanet->Graphic->RingKind > 0) {
                        if (ExistingRing > 0) {
                            AllowRing = false;
                        }
                        ExistingRing = PreviousPlanet->Graphic->RingKind;
                    }
                }
                if (AllowRing && static_cast<std::uint8_t>(IsSolar ^ 1)) {
                    if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.8L && GraphicRadius > 90 && static_cast<std::uint8_t>(pas::in_set<1, 1, 4, 5>(ExistingRing) ^ 1) && OwnerId != aGalaxyStruct::oiUninhabited) {
                        I = 0;
                        do {
                            ++I;
                            RingKind = aMyFunction::NextRandomIntRange(1, 9, RandomState);
                        } while (!(pas::in_set<1, 1, 4, 5>(RingKind) || I > 200));
                        Graphic->SetRingKind(RingKind);
                    } else if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.4L && GraphicRadius > 90) {
                        if (!pas::in_set<21, 22>(ExistingRing)) {
                            std::uint8_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(21, 22, RandomState);
                            SE_Planet::TPlanetSE* graphic_11 = Graphic;
                            graphic_11->SetRingKind(nextRandomIntRange_4);
                        }
                    } else if (OwnerId != aGalaxyStruct::oiUninhabited && GraphicRadius > 70 && aMyFunction::NextRandomUnitFloat(RandomState) < 0.7L) {
                        if (!pas::in_range(ExistingRing, 1, 9)) {
                            std::uint8_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(1, 9, RandomState);
                            SE_Planet::TPlanetSE* graphic_12 = Graphic;
                            graphic_12->SetRingKind(nextRandomIntRange_5);
                        }
                    } else {
                        Graphic->SetRingKind(0);
                    }
                } else {
                    Graphic->SetRingKind(0);
                }
                Radius = GraphicRadius;
                if (pas::list_count(Star->Planets) == 0) {
                    Orbit.Radius = Radius + Star->SystemRadius + 350;
                } else {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
                }
                {
                    pas::Extended cpp_left_3 = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
                    OrbitalVelocity = cpp_left_3 * (4.5L - pas::real_divide(pas::list_count(Star->Planets), 2.0L));
                }
                Orbit.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
                Graphic->SetPosition(aMyFunction::PolarToPoint(Orbit));
                {
                    std::uint32_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(60, 100, RandomState);
                    SE_Planet::TPlanetSE* graphic_13 = Graphic;
                    graphic_13->SetRotationTimerInterval(nextRandomIntRange_6);
                }
                {
                    std::int32_t cpp_arg_15 = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
                    SE_Planet::TPlanetSE* graphic_14 = Graphic;
                    graphic_14->SetSurfaceMapStep(cpp_arg_15);
                }
                Graphic->OrbitalVelocity = OrbitalVelocity;
                SatelliteConfig = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get());
                if (Graphic->RingKind == 0) {
                    MinOrbitRadius = System::Round(Radius * 1.3L);
                } else {
                    MinOrbitRadius = System::Round(Radius * 1.5L);
                }
                MaxOrbitRadius = Radius * 2;
                if (Graphic->RingKind == 0) {
                    SatelliteCount = aMyFunction::NextRandomIntRange(1, 4, RandomState);
                } else if (Graphic->RingKind < 20) {
                    SatelliteCount = aMyFunction::NextRandomIntRange(0, 4, RandomState);
                } else {
                    SatelliteCount = 0;
                }
                if (pas::list_count(Star->Planets) > 0) {
                    PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
                    if (pas::list_count(PreviousPlanet->Satellites) > 0) {
                        if (Graphic->RingKind > 0) {
                            SatelliteCount = 0;
                        } else if (OwnerId == aGalaxyStruct::oiUninhabited) {
                            SatelliteCount = 0;
                        } else {
                            SatelliteCount = std::min<std::int32_t>(SatelliteCount, 1);
                        }
                    }
                }
                PreviousExtent = 0.0;
                if (SatelliteCount > 0) {
                    for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_9.next(I); ) {
                        if (SatelliteCount == 1) {
                            SatelliteRadius = MinOrbitRadius;
                        } else {
                            SatelliteRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                        }
                        if (I <= 0 || SatelliteRadius >= PreviousExtent) {
                            Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                            pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                            {
                                SE_Space::TObjectSE* cpp_arg_16 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, pas::concat_wide({u"Sputnik.", ([&] {
                                    std::int32_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(0, SatelliteConfig->GetBlockCount() - 1, RandomState);
                                    EC_BlockPar::TBlockParEC* satelliteConfig = SatelliteConfig;
                                    return satelliteConfig->GetBlockNameByIndex(nextRandomIntRange_7);
                                }())}), ClassesImports::Point(0, 0));
                                pas::Var<SE_Space::TObjectSE*> graphic_15 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                                SE_Space::RetainSpaceObject(graphic_15, cpp_arg_16);
                            }
                            Satellite->Graphic->DepthOrder = I;
                            Satellite->Graphic->OrbitCenter = GetPosition();
                            if (SatelliteCount == 1) {
                                Satellite->Graphic->OrbitRadius = MinOrbitRadius;
                            } else {
                                Satellite->Graphic->OrbitRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                            }
                            Satellite->Graphic->OrbitAngle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
                            Satellite->Graphic->OrbitAngleStep = 1.0L + aMyFunction::NextRandomUnitFloat(RandomState);
                            Satellite->Graphic->OrbitTimerInterval = aMyFunction::NextRandomIntRange(28, 35, RandomState);
                            Satellite->Graphic->OrbitInclination = aMyFunction::NextRandomIntRange(50, 120, RandomState);
                            Satellite->Graphic->OrbitRotation = aMyFunction::NextRandomIntRange(200, 350, RandomState);
                            {
                                double cpp_arg_17 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), SatelliteCount);
                                double generatedSatelliteBaseRadius = GlobalsV::GeneratedSatelliteBaseRadius;
                                double cpp_arg_18 = GlobalsV::GeneratedSatelliteBaseRadius * 2;
                                Satellite->Graphic->MinDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_17, 0.0, 1.0, generatedSatelliteBaseRadius, cpp_arg_18));
                            }
                            {
                                std::int32_t min_3 = std::min<std::int32_t>(GlobalsV::MaximumSatelliteTemplateRadius, Satellite->Graphic->MinDisplayRadius * 2);
                                {
                                    double cpp_arg_19 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), 1.0L);
                                    double cpp_arg_20 = Satellite->Graphic->MinDisplayRadius * 1.3L;
                                    Satellite->Graphic->MaxDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_19, 0.0, 1.0, cpp_arg_20, min_3));
                                }
                            }
                            PreviousExtent = static_cast<long double>(Satellite->Graphic->OrbitRadius) + Satellite->Graphic->MaxDisplayRadius / 2;
                            Satellite->Graphic->RotationTimerInterval = 25u;
                            Satellite->Graphic->SurfaceMapStep = (I % 2 * 2 - 1) * 2;
                        }
                    }
                }
            }
            Government = static_cast<aGalaxyStruct::TPlanetGovernment>(aMyFunction::NextRandomIntRange(0, 4, RandomState));
            GovernmentRoll = aMyFunction::NextRandomIntRange(0, 100, RandomState);
            for (auto cpp_range_10 = pas::for_downto<aGalaxyStruct::TPlanetGovernment>(aGalaxyStruct::pgDemocracy, aGalaxyStruct::pgAnarchy); cpp_range_10.next(GovernmentCandidate); ) {
                if (aConst::PlanetRaceMarket[RaceId].GovernmentRollThresholds[GovernmentCandidate] <= GovernmentRoll) {
                    Government = GovernmentCandidate;
                    break;
                }
            }
            EconomyRoll = aMyFunction::NextRandomIntRange(1, aGalaxy::Galaxy->GetAgriculturalPlanetWeight() + aGalaxy::Galaxy->GetMixedPlanetWeight() + aGalaxy::Galaxy->GetIndustrialPlanetWeight(), RandomState);
            if (EconomyRoll <= aGalaxy::Galaxy->GetAgriculturalPlanetWeight()) {
                Economy = aGalaxyStruct::peAgricultural;
            } else if (EconomyRoll <= aGalaxy::Galaxy->GetAgriculturalPlanetWeight() + aGalaxy::Galaxy->GetAgriculturalPlanetWeight()) {
                Economy = aGalaxyStruct::peMixed;
            } else {
                Economy = aGalaxyStruct::peIndustrial;
            }
            Population = CalculateBasePopulation();
        }
        for (Invention = static_cast<std::uint8_t>(0); Invention <= static_cast<std::uint8_t>(19); ++Invention) {
            InventionLevels[Invention] = aConst::PlanetInventionInfo[Invention].InitialLevel;
        }
        CurrentInvention = 0;
        CurrentInventionPoints = 0.0f;
        ResearchLevelPercent = 30;
        BoostInventionLevels(aConst::PlanetRaceMarket[RaceId].InitialInventionBoostCount);
        ResearchLevelPercent = aMyFunction::NextRandomIntRange(20, 40, RandomState);
        ResearchLevelStep = aMyFunction::NextRandomIntRange(5, 10, RandomState);
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Goods[Good].Count = aMyFunction::NextRandomIntRange(aConst::GoodsMarket[Good].BaseStock / 2, aConst::GoodsMarket[Good].BaseStock, RandomState);
            Goods[Good].PriceState = aConst::GoodsMarket[Good].AveragePrice;
            Goods[Good].PurchasePrice = System::Round(Goods[Good].PriceState);
            Goods[Good].BaseSalePrice = System::Round(Goods[Good].PriceState * 0.98L - 1.0L);
            GoodsScarcityTicks[Good] = 0;
            GoodsSurplusTicks[Good] = 0;
        }
        TextQuestId = -1;
        Money = System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0E+4, 1.0E+5));
        HomeRangerCount = 0;
        HomeTransportCount = 0;
        if (OwnerId != aGalaxyStruct::oiUninhabited) {
            for (auto cpp_range_11 = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_Weapon1); cpp_range_11.next(ItemType); ) {
                switch (ItemType) {
                    case aConst::t_Hull: {
                        for (auto cpp_range_12 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 5, RandomState)); cpp_range_12.next(I); ) {
                            Item = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            HullType = aMyFunction::NextRandomIntRange(0, 5, RandomState);
                            ItemOwner = aConst::RaceToOwner(RaceId);
                            Series = aGalaxy::Galaxy->SelectHullSeries(ItemOwner, HullType, 1, 100);
                            {
                                std::uint8_t nextRandomIntRange_8 = aMyFunction::NextRandomIntRange(1, InventionLevels[0], RandomState);
                                std::int32_t round = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[4]);
                                std::int32_t round_2 = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_9 = aMyFunction::NextRandomIntRange(round_2, round, RandomState);
                                aItem::THull* cpp_arg_21 = pas::checked_cast<aItem::THull*>(Item);
                                cpp_arg_21->Init(nextRandomIntRange_9, nextRandomIntRange_8, ItemOwner, HullType, Series, false);
                            }
                        }
                        break;
                    }
                    case aConst::t_FuelTanks: {
                        for (auto cpp_range_13 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_13.next(I); ) {
                            Item = pas::construct_call<aItem::TFuelTanks>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId = OwnerId;
                                std::uint8_t nextRandomIntRange_10 = aMyFunction::NextRandomIntRange(1, InventionLevels[1], RandomState);
                                std::int32_t round_3 = System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_4 = System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_11 = aMyFunction::NextRandomIntRange(round_4, round_3, RandomState);
                                aItem::TFuelTanks* cpp_arg_22 = pas::checked_cast<aItem::TFuelTanks*>(Item);
                                cpp_arg_22->Init(nextRandomIntRange_11, nextRandomIntRange_10, ownerId);
                            }
                        }
                        break;
                    }
                    case aConst::t_Engine: {
                        for (auto cpp_range_14 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 3, RandomState)); cpp_range_14.next(I); ) {
                            Item = pas::construct_call<aItem::TEngine>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_2 = OwnerId;
                                std::uint8_t nextRandomIntRange_12 = aMyFunction::NextRandomIntRange(1, InventionLevels[2], RandomState);
                                std::int32_t round_5 = System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_6 = System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_13 = aMyFunction::NextRandomIntRange(round_6, round_5, RandomState);
                                aItem::TEngine* cpp_arg_23 = pas::checked_cast<aItem::TEngine*>(Item);
                                cpp_arg_23->Init(nextRandomIntRange_13, nextRandomIntRange_12, ownerId_2);
                            }
                        }
                        break;
                    }
                    case aConst::t_Radar: {
                        for (auto cpp_range_15 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_15.next(I); ) {
                            Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_3 = OwnerId;
                                std::uint8_t nextRandomIntRange_14 = aMyFunction::NextRandomIntRange(1, InventionLevels[3], RandomState);
                                std::int32_t round_7 = System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_8 = System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_15 = aMyFunction::NextRandomIntRange(round_8, round_7, RandomState);
                                aItem::TRadar* cpp_arg_24 = pas::checked_cast<aItem::TRadar*>(Item);
                                cpp_arg_24->Init(nextRandomIntRange_15, nextRandomIntRange_14, ownerId_3);
                            }
                        }
                        break;
                    }
                    case aConst::t_Scaner: {
                        for (auto cpp_range_16 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_16.next(I); ) {
                            Item = pas::construct_call<aItem::TScaner>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_4 = OwnerId;
                                std::uint8_t nextRandomIntRange_16 = aMyFunction::NextRandomIntRange(1, InventionLevels[4], RandomState);
                                std::int32_t round_9 = System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_10 = System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_17 = aMyFunction::NextRandomIntRange(round_10, round_9, RandomState);
                                aItem::TScaner* cpp_arg_25 = pas::checked_cast<aItem::TScaner*>(Item);
                                cpp_arg_25->Init(nextRandomIntRange_17, nextRandomIntRange_16, ownerId_4);
                            }
                        }
                        break;
                    }
                    case aConst::t_RepairRobot: {
                        for (auto cpp_range_17 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_17.next(I); ) {
                            Item = pas::construct_call<aItem::TRepairRobot>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_5 = OwnerId;
                                std::uint8_t nextRandomIntRange_18 = aMyFunction::NextRandomIntRange(1, InventionLevels[5], RandomState);
                                std::int32_t round_11 = System::Round(static_cast<long double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_12 = System::Round(static_cast<long double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_19 = aMyFunction::NextRandomIntRange(round_12, round_11, RandomState);
                                aItem::TRepairRobot* cpp_arg_26 = pas::checked_cast<aItem::TRepairRobot*>(Item);
                                cpp_arg_26->Init(nextRandomIntRange_19, nextRandomIntRange_18, ownerId_5);
                            }
                        }
                        break;
                    }
                    case aConst::t_CargoHook: {
                        for (auto cpp_range_18 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_18.next(I); ) {
                            Item = pas::construct_call<aItem::TCargoHook>(aItem::TCargoHook_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_6 = OwnerId;
                                std::uint8_t nextRandomIntRange_20 = aMyFunction::NextRandomIntRange(1, InventionLevels[6], RandomState);
                                std::int32_t round_13 = System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_14 = System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_21 = aMyFunction::NextRandomIntRange(round_14, round_13, RandomState);
                                aItem::TCargoHook* cpp_arg_27 = pas::checked_cast<aItem::TCargoHook*>(Item);
                                cpp_arg_27->Init(nextRandomIntRange_21, nextRandomIntRange_20, ownerId_6);
                            }
                        }
                        break;
                    }
                    case aConst::t_DefGenerator: {
                        for (auto cpp_range_19 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_19.next(I); ) {
                            Item = pas::construct_call<aItem::TDefGenerator>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_7 = OwnerId;
                                std::uint8_t nextRandomIntRange_22 = aMyFunction::NextRandomIntRange(1, InventionLevels[7], RandomState);
                                std::int32_t round_15 = System::Round(static_cast<long double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_16 = System::Round(static_cast<long double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_23 = aMyFunction::NextRandomIntRange(round_16, round_15, RandomState);
                                aItem::TDefGenerator* cpp_arg_28 = pas::checked_cast<aItem::TDefGenerator*>(Item);
                                cpp_arg_28->Init(nextRandomIntRange_23, nextRandomIntRange_22, ownerId_7);
                            }
                        }
                        break;
                    }
                    case aConst::t_Weapon1: {
                        for (auto cpp_range_20 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(2, InventionLevels[7] + 2, RandomState)); cpp_range_20.next(I); ) {
                            {
                                std::uint8_t cpp_arg_29 = InventionLevels[7];
                                std::uint32_t randomIntRange = aMyFunction::RandomIntRange(1, 100000);
                                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                                WeaponInfo = galaxy->SelectWeaponInfo(randomIntRange, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), cpp_arg_29, 1);
                            }
                            GeneratedWeapon = ([&] {
                                aGalaxyStruct::TOwnerId ownerId_8 = OwnerId;
                                std::int32_t nextRandomIntRange_24 = aMyFunction::NextRandomIntRange(1, InventionLevels[7], RandomState);
                                std::int32_t nextRandomIntRange_25 = ([&] {
                                    std::int32_t round_17 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * aConst::EquipmentSizeFactors[1]);
                                    std::int32_t round_18 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * aConst::EquipmentSizeFactors[5]);
                                    return aMyFunction::NextRandomIntRange(round_18, round_17, RandomState);
                                }());
                                return aItem::CreateGeneratedWeapon(WeaponInfo, nextRandomIntRange_25, nextRandomIntRange_24, ownerId_8);
                            }());
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(GeneratedWeapon));
                        }
                        break;
                    }
                }
            }
        }
        WaterTiles = System::Round(Graphic->SpaceConfigValues[0]);
        WaterExplored = 0;
        LandTiles = System::Round(Graphic->SpaceConfigValues[1]);
        LandExplored = 0;
        HillTiles = System::Round(Graphic->SpaceConfigValues[2]);
        HillExplored = 0;
        ProbeOrbitCount = System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0));
        if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.5L) {
            ++ProbeOrbitCount;
        }
        if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
            ++ProbeOrbitCount;
        }
        if (OwnerId == aGalaxyStruct::oiUninhabited && GetUnexploredSurfaceTileCount() > 50) {
            for (auto cpp_range_21 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0)), RandomState)); cpp_range_21.next(I); ) {
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                {
                    std::uint32_t cpp_arg_30 = 767 + aMyFunction::NextRandomIntRange(1, 3000, RandomState);
                    aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(RaceId);
                    Loot = aItem::CreateRandomLootItem(aItem::ilpTreasure, raceToOwner, cpp_arg_30);
                }
                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Loot)) {
                    static_cast<aTranclucator::TTranclucator*>(artefactTranclucator->Ship)->OwnerShip = nullptr;
                }
                AddSurfaceLootEntry(Loot);
            }
            for (auto cpp_range_22 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0)), RandomState)); cpp_range_22.next(I); ) {
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                {
                    std::uint32_t randomState_3 = RandomState;
                    std::uint8_t nextRandomIntRange_26 = aMyFunction::NextRandomIntRange(0, 50, RandomState);
                    aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(nextRandomIntRange_26, 100, randomState_3, nullptr);
                }
                Module = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                Module->Init(ModuleIndex);
                AddSurfaceLootEntry(Module);
            }
            for (auto cpp_range_23 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 4.0)))); cpp_range_23.next(I); ) {
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.4L) {
                    continue;
                }
                Cistern = pas::construct_call<aItem::TCistern>(aItem::TEquipment_Create);
                Count = aMyFunction::NextRandomIntRange(5, 20, RandomState);
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.2L) {
                    Count = aMyFunction::RoundAndTruncateToFives(Count * 2);
                    if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.1L) {
                        Count = aMyFunction::RoundAndTruncateToFives(Count * 2);
                    }
                }
                {
                    aGalaxyStruct::TOwnerId raceToOwner_2 = aConst::RaceToOwner(RaceId);
                    std::int32_t nextRandomIntRange_27 = aMyFunction::NextRandomIntRange(0, Count, RandomState);
                    Cistern->Init(nextRandomIntRange_27, Count, raceToOwner_2);
                }
                AddSurfaceLootEntry(Cistern);
            }
            MinLevel = 1;
            MaxLevel = 5;
            {
                std::int32_t nextRandomIntRange_28 = aMyFunction::NextRandomIntRange(2, 4, RandomState);
                std::uint32_t& randomState_4 = RandomState;
                WeaponTechLevel = aMyFunction::NextRandomIntRange(1, nextRandomIntRange_28, randomState_4);
            }
            MinSizeFactor = aConst::EquipmentSizeFactors[5];
            MaxSizeFactor = aConst::EquipmentSizeFactors[1];
            for (auto cpp_range_24 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 3.0)))); cpp_range_24.next(I); ) {
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.75L) {
                    continue;
                }
                Count = 0;
                Item = nullptr;
                while (true) {
                    ++Count;
                    if (aMyFunction::NextRandomIntRange(1, 130, RandomState) > 70) {
                        WeaponInfo = aGalaxy::Galaxy->SelectWeaponInfo(RandomState, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), std::min<std::int32_t>(WeaponTechLevel + 2, 8), std::max<std::int32_t>(1, WeaponTechLevel - 1));
                        Weight = ([&] {
                            std::int32_t round_19 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * MaxSizeFactor);
                            std::int32_t round_20 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * MinSizeFactor);
                            return aMyFunction::NextRandomIntRange(round_20, round_19, RandomState);
                        }());
                        Level = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        Item = aItem::CreateGeneratedWeapon(WeaponInfo, Weight, Level, aGalaxyStruct::oiUninhabited);
                    } else {
                        ItemType = static_cast<aConst::TItemType>(aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}})));
                        {
                            std::int32_t round_21 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MaxSizeFactor);
                            std::int32_t round_22 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MinSizeFactor);
                            Weight = aMyFunction::NextRandomIntRange(round_22, round_21, RandomState);
                        }
                        Level = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        Item = aItem::CreateGeneratedEquipment(ItemType, Weight, Level, aGalaxyStruct::oiUninhabited);
                    }
                    if (Item->Cost < 5000 || Item->Cost < 7.5E+3L && Count > 2 || Item->Cost < 10000 && Count > 3 || Count > 4) {
                        break;
                    }
                    pas::free(Item);
                }
                Item->ConditionPercent = aMyFunction::SeededRandomFloatRange(Item->Id, 1.0E+1, 1.0E+2);
                AddSurfaceLootEntry(Item);
            }
            for (auto cpp_range_25 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 5.0)))); cpp_range_25.next(I); ) {
                if (I > 2 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.4L) {
                    continue;
                }
                ItemType = static_cast<aConst::TItemType>(aMyFunction::NextRandomIntRange(0, 7, RandomState));
                Count = aMyFunction::NextRandomIntRange(std::max<std::int32_t>(1, aConst::GoodsMarket[ItemType].BaseStock / 20), System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, aConst::GoodsMarket[ItemType].BaseStock / 15, aConst::GoodsMarket[ItemType].BaseStock / 7)), RandomState);
                SavedRandomState = RandomState;
                if (Count < 10) {
                    Part = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                } else {
                    Part = aMyFunction::NextRandomIntRange(1, 4, RandomState);
                }
                for (auto cpp_range_26 = pas::for_to<std::int32_t>(1, Part); cpp_range_26.next(Part); ) {
                    GoodsItem = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                    Quantity = pas::idiv(Count, aMyFunction::SeededRandomIntRange(1, 5, RandomState * Part * static_cast<std::uint8_t>(ItemType))) + 1;
                    RandomState = SavedRandomState;
                    GoodsItem->Init(ItemType, Quantity);
                    {
                        std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(2, 5, RandomState * Part * static_cast<std::uint8_t>(ItemType) * 3);
                        GoodsItem->Cost = pas::idiv(GoodsItem->Cost, cpp_right);
                    }
                    AddSurfaceLootEntry(GoodsItem);
                }
            }
            NormalizeSurfaceLootEntries();
        }
        HasPlayerLanded = false;
        UpdateOwnerFlags();
        for (auto cpp_range_27 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(aGalaxy::Galaxy->CustomRules.ExtraInventions)); cpp_range_27.next(I); ) {
            CurrentInventionPoints = 101.0f;
            AdvanceInventionProgress();
        }
    }

    // Only sets CurrentStar, OwnerId=5 and all invention levels to 8; used by the separate Dominator spawn planet.
    void TPlanet::InitDominatorSpawnProxy(aGalaxy::TStar* Star) {
        std::uint8_t Index{};
        CurrentStar = Star;
        OwnerId = aGalaxyStruct::oiDominator;
        for (Index = static_cast<std::uint8_t>(0); Index <= static_cast<std::uint8_t>(19); ++Index) {
            InventionLevels[Index] = 8;
        }
    }

    // Creates graphics, surface terrain, loot and initial market/research state; caller owns planet registration.
    void TPlanet::InitGeneratedUninhabited(aGalaxy::TStar* Star) {
        TPlanet* PreviousPlanet{};
        std::uint8_t Invention{};
        std::uint8_t Good{};
        std::int32_t I{};
        std::int32_t Part{};
        std::int32_t ModuleIndex{};
        std::uint32_t SavedRandomState{};
        double SatelliteRadius{};
        double MinOrbitRadius{};
        aGalaxyStruct::TOwnerId ItemOwner{};
        pas::WideString UnusedText{};
        TSputnik* Satellite{};
        std::int32_t SatelliteCount{};
        EC_BlockPar::TBlockParEC* SatelliteConfig{};
        aItem::TEquipment* Item{};
        aConst::TItemType ItemType{};
        std::int32_t Series{};
        std::uint8_t HullType{};
        aItem::TEquipmentWithActCode* Loot{};
        aItem::TMicroModule* Module{};
        aItem::TCistern* Cistern{};
        aItem::TGoods* GoodsItem{};
        aRanger::TRanger* Ranger{};
        std::int32_t Weight{};
        std::int32_t Level{};
        aConst::PWeaponInfo WeaponInfo{};
        float MinSizeFactor{};
        float MaxSizeFactor{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t WeaponTechLevel{};
        aItem::TWeapon* GeneratedWeapon{};
        CurrentStar = Star;
        OwnerId = aGalaxyStruct::oiUninhabited;
        RaceId = aGalaxyStruct::oiMaloc;
        Name = u"New Planet"_w;
        std::int32_t Count = Globals::PlanetSpaceTemplates.length() - 1 + 1;
        std::int32_t Quantity = aMyFunction::RandomIntRange(0, Count - 1);
        while (Globals::PlanetSpaceTemplates[Quantity].Style == 1) {
            Quantity = aMyFunction::RandomIntRange(0, Count - 1);
        }
        SpriteTemplateIndex = Quantity;
        GraphicRadius = Globals::PlanetSpaceTemplates[Quantity].Radius;
        {
            SE_Space::TObjectSE* cpp_arg = pas::construct_call<SE_Planet::TPlanetSE>(SE_Planet::TPlanetSE_Create);
            pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Graphic);
            SE_Space::RetainSpaceObject(graphic, cpp_arg);
        }
        Globals::PlanetSpaceTemplates[Quantity].SpaceObject->CopyTo(Graphic);
        GraphName = Graphic->GraphKey;
        std::uint8_t AllowRing = true;
        std::int32_t ExistingRing = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range.next(I); ) {
            PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, I);
            if (PreviousPlanet->Graphic->RingKind > 0) {
                if (ExistingRing > 0) {
                    AllowRing = false;
                }
                ExistingRing = PreviousPlanet->Graphic->RingKind;
            }
        }
        if (AllowRing && aMyFunction::NextRandomUnitFloat(RandomState) < 0.4L && GraphicRadius > 90) {
            if (!pas::in_set<21, 22>(ExistingRing)) {
                std::uint8_t nextRandomIntRange = aMyFunction::NextRandomIntRange(21, 22, RandomState);
                SE_Planet::TPlanetSE* graphic_2 = Graphic;
                graphic_2->SetRingKind(nextRandomIntRange);
            }
        } else {
            Graphic->SetRingKind(0);
        }
        Radius = GraphicRadius;
        PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
        Orbit.Radius = static_cast<long double>(PreviousPlanet->Orbit.Radius) + PreviousPlanet->Radius + Radius + 2.0E+2L + System::Round(2.0E+2L - aMyFunction::RemapClamped(pas::list_count(Star->Planets), 1.0, 6.0, 0.0, 2.0E+2));
        {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
            OrbitalVelocity = cpp_left * (4.5L - pas::real_divide(pas::list_count(Star->Planets), 2.0L));
        }
        Orbit.AngleDegrees = aMyFunction::NextRandomIntRange(0, 359, RandomState);
        Graphic->SetPosition(aMyFunction::PolarToPoint(Orbit));
        {
            std::uint32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(60, 100, RandomState);
            SE_Planet::TPlanetSE* graphic_3 = Graphic;
            graphic_3->SetRotationTimerInterval(nextRandomIntRange_2);
        }
        {
            std::int32_t cpp_arg_2 = aMyFunction::NextRandomIntRange(0, 1, RandomState) * 2 - 1;
            SE_Planet::TPlanetSE* graphic_4 = Graphic;
            graphic_4->SetSurfaceMapStep(cpp_arg_2);
        }
        Graphic->OrbitalVelocity = OrbitalVelocity;
        SatelliteConfig = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Sputnik"_wref.get());
        if (Graphic->RingKind == 0) {
            MinOrbitRadius = System::Round(Radius * 1.3L);
        } else {
            MinOrbitRadius = System::Round(Radius * 1.5L);
        }
        double MaxOrbitRadius = Radius * 2;
        if (Graphic->RingKind == 0) {
            SatelliteCount = aMyFunction::NextRandomIntRange(1, 4, RandomState);
        } else if (Graphic->RingKind < 20) {
            SatelliteCount = aMyFunction::NextRandomIntRange(0, 4, RandomState);
        } else {
            SatelliteCount = 0;
        }
        if (pas::list_count(Star->Planets) > 0) {
            PreviousPlanet = pas::list_at<TPlanet>(Star->Planets, pas::list_count(Star->Planets) - 1);
            if (pas::list_count(PreviousPlanet->Satellites) > 0) {
                SatelliteCount = 0;
            }
        }
        double PreviousExtent = 0.0;
        if (SatelliteCount > 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SatelliteCount - 1); cpp_range_2.next(I); ) {
                if (SatelliteCount == 1) {
                    SatelliteRadius = MinOrbitRadius;
                } else {
                    SatelliteRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                }
                if (I <= 0 || SatelliteRadius >= PreviousExtent) {
                    Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                    pas::list_add(Satellites, reinterpret_cast<void*>(Satellite));
                    {
                        SE_Space::TObjectSE* cpp_arg_3 = pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, pas::concat_wide({u"Sputnik.", ([&] {
                            std::int32_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(0, SatelliteConfig->GetBlockCount() - 1, RandomState);
                            EC_BlockPar::TBlockParEC* satelliteConfig = SatelliteConfig;
                            return satelliteConfig->GetBlockNameByIndex(nextRandomIntRange_3);
                        }())}), ClassesImports::Point(0, 0));
                        pas::Var<SE_Space::TObjectSE*> graphic_5 = pas::Var<SE_Space::TObjectSE*>(&Satellite->Graphic);
                        SE_Space::RetainSpaceObject(graphic_5, cpp_arg_3);
                    }
                    Satellite->Graphic->DepthOrder = I;
                    Satellite->Graphic->OrbitCenter = GetPosition();
                    if (SatelliteCount == 1) {
                        Satellite->Graphic->OrbitRadius = MinOrbitRadius;
                    } else {
                        Satellite->Graphic->OrbitRadius = System::Round(aMyFunction::RemapClamped(I, 0.0, 3.0, MinOrbitRadius, MaxOrbitRadius));
                    }
                    Satellite->Graphic->OrbitAngle = aMyFunction::NextRandomIntRange(0, 360, RandomState);
                    Satellite->Graphic->OrbitAngleStep = 1.0L + aMyFunction::NextRandomUnitFloat(RandomState);
                    Satellite->Graphic->OrbitTimerInterval = aMyFunction::NextRandomIntRange(28, 35, RandomState);
                    Satellite->Graphic->OrbitInclination = aMyFunction::NextRandomIntRange(50, 120, RandomState);
                    Satellite->Graphic->OrbitRotation = aMyFunction::NextRandomIntRange(200, 350, RandomState);
                    {
                        double cpp_arg_4 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), SatelliteCount);
                        double generatedSatelliteBaseRadius = GlobalsV::GeneratedSatelliteBaseRadius;
                        double cpp_arg_5 = GlobalsV::GeneratedSatelliteBaseRadius * 2;
                        Satellite->Graphic->MinDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_4, 0.0, 1.0, generatedSatelliteBaseRadius, cpp_arg_5));
                    }
                    {
                        std::int32_t min = std::min<std::int32_t>(GlobalsV::MaximumSatelliteTemplateRadius, Satellite->Graphic->MinDisplayRadius * 2);
                        {
                            double cpp_arg_6 = pas::real_divide(aMyFunction::NextRandomUnitFloat(RandomState), 1.0L);
                            double cpp_arg_7 = Satellite->Graphic->MinDisplayRadius * 1.3L;
                            Satellite->Graphic->MaxDisplayRadius = System::Round(aMyFunction::RemapClamped(cpp_arg_6, 0.0, 1.0, cpp_arg_7, min));
                        }
                    }
                    PreviousExtent = static_cast<long double>(Satellite->Graphic->OrbitRadius) + Satellite->Graphic->MaxDisplayRadius / 2;
                    Satellite->Graphic->RotationTimerInterval = 25u;
                    Satellite->Graphic->SurfaceMapStep = (I % 2 * 2 - 1) * 2;
                }
            }
        }
        Government = aGalaxyStruct::pgAnarchy;
        Economy = aGalaxyStruct::peMixed;
        Population = CalculateBasePopulation();
        for (Invention = static_cast<std::uint8_t>(0); Invention <= static_cast<std::uint8_t>(19); ++Invention) {
            InventionLevels[Invention] = aConst::PlanetInventionInfo[Invention].InitialLevel;
        }
        CurrentInvention = 0;
        CurrentInventionPoints = 0.0f;
        ResearchLevelPercent = 30;
        BoostInventionLevels(aConst::PlanetRaceMarket[RaceId].InitialInventionBoostCount);
        ResearchLevelPercent = aMyFunction::NextRandomIntRange(20, 40, RandomState);
        ResearchLevelStep = aMyFunction::NextRandomIntRange(5, 10, RandomState);
        for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
            Goods[Good].Count = aMyFunction::NextRandomIntRange(aConst::GoodsMarket[Good].BaseStock / 2, aConst::GoodsMarket[Good].BaseStock, RandomState);
            Goods[Good].PriceState = aConst::GoodsMarket[Good].AveragePrice;
            Goods[Good].PurchasePrice = System::Round(Goods[Good].PriceState);
            Goods[Good].BaseSalePrice = System::Round(Goods[Good].PriceState * 0.98L - 1.0L);
            GoodsScarcityTicks[Good] = 0;
            GoodsSurplusTicks[Good] = 0;
        }
        TextQuestId = -1;
        Money = System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0E+4, 1.0E+5));
        HomeRangerCount = 0;
        HomeTransportCount = 0;
        // Kept even though OwnerId was assigned 6 above: the original emits this stock-generation branch.
        if (OwnerId != aGalaxyStruct::oiUninhabited) {
            for (auto cpp_range_3 = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_Weapon1); cpp_range_3.next(ItemType); ) {
                switch (ItemType) {
                    case aConst::t_Hull: {
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 5, RandomState)); cpp_range_4.next(I); ) {
                            Item = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            HullType = aMyFunction::NextRandomIntRange(0, 5, RandomState);
                            ItemOwner = aConst::RaceToOwner(RaceId);
                            Series = aGalaxy::Galaxy->SelectHullSeries(ItemOwner, HullType, 1, 100);
                            {
                                std::uint8_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(1, InventionLevels[0], RandomState);
                                std::int32_t round = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[4]);
                                std::int32_t round_2 = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_5 = aMyFunction::NextRandomIntRange(round_2, round, RandomState);
                                aItem::THull* cpp_arg_8 = pas::checked_cast<aItem::THull*>(Item);
                                cpp_arg_8->Init(nextRandomIntRange_5, nextRandomIntRange_4, ItemOwner, HullType, Series, false);
                            }
                        }
                        break;
                    }
                    case aConst::t_FuelTanks: {
                        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_5.next(I); ) {
                            Item = pas::construct_call<aItem::TFuelTanks>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId = OwnerId;
                                std::uint8_t nextRandomIntRange_6 = aMyFunction::NextRandomIntRange(1, InventionLevels[1], RandomState);
                                std::int32_t round_3 = System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_4 = System::Round(static_cast<long double>(aConst::FuelTanksBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_7 = aMyFunction::NextRandomIntRange(round_4, round_3, RandomState);
                                aItem::TFuelTanks* cpp_arg_9 = pas::checked_cast<aItem::TFuelTanks*>(Item);
                                cpp_arg_9->Init(nextRandomIntRange_7, nextRandomIntRange_6, ownerId);
                            }
                        }
                        break;
                    }
                    case aConst::t_Engine: {
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 3, RandomState)); cpp_range_6.next(I); ) {
                            Item = pas::construct_call<aItem::TEngine>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_2 = OwnerId;
                                std::uint8_t nextRandomIntRange_8 = aMyFunction::NextRandomIntRange(1, InventionLevels[2], RandomState);
                                std::int32_t round_5 = System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_6 = System::Round(static_cast<long double>(aConst::EngineBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_9 = aMyFunction::NextRandomIntRange(round_6, round_5, RandomState);
                                aItem::TEngine* cpp_arg_10 = pas::checked_cast<aItem::TEngine*>(Item);
                                cpp_arg_10->Init(nextRandomIntRange_9, nextRandomIntRange_8, ownerId_2);
                            }
                        }
                        break;
                    }
                    case aConst::t_Radar: {
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_7.next(I); ) {
                            Item = pas::construct_call<aItem::TRadar>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_3 = OwnerId;
                                std::uint8_t nextRandomIntRange_10 = aMyFunction::NextRandomIntRange(1, InventionLevels[3], RandomState);
                                std::int32_t round_7 = System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_8 = System::Round(static_cast<long double>(aConst::RadarBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_11 = aMyFunction::NextRandomIntRange(round_8, round_7, RandomState);
                                aItem::TRadar* cpp_arg_11 = pas::checked_cast<aItem::TRadar*>(Item);
                                cpp_arg_11->Init(nextRandomIntRange_11, nextRandomIntRange_10, ownerId_3);
                            }
                        }
                        break;
                    }
                    case aConst::t_Scaner: {
                        for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_8.next(I); ) {
                            Item = pas::construct_call<aItem::TScaner>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_4 = OwnerId;
                                std::uint8_t nextRandomIntRange_12 = aMyFunction::NextRandomIntRange(1, InventionLevels[4], RandomState);
                                std::int32_t round_9 = System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_10 = System::Round(static_cast<long double>(aConst::ScannerBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_13 = aMyFunction::NextRandomIntRange(round_10, round_9, RandomState);
                                aItem::TScaner* cpp_arg_12 = pas::checked_cast<aItem::TScaner*>(Item);
                                cpp_arg_12->Init(nextRandomIntRange_13, nextRandomIntRange_12, ownerId_4);
                            }
                        }
                        break;
                    }
                    case aConst::t_RepairRobot: {
                        for (auto cpp_range_9 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_9.next(I); ) {
                            Item = pas::construct_call<aItem::TRepairRobot>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_5 = OwnerId;
                                std::uint8_t nextRandomIntRange_14 = aMyFunction::NextRandomIntRange(1, InventionLevels[5], RandomState);
                                std::int32_t round_11 = System::Round(static_cast<long double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_12 = System::Round(static_cast<long double>(aConst::RepairRobotBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_15 = aMyFunction::NextRandomIntRange(round_12, round_11, RandomState);
                                aItem::TRepairRobot* cpp_arg_13 = pas::checked_cast<aItem::TRepairRobot*>(Item);
                                cpp_arg_13->Init(nextRandomIntRange_15, nextRandomIntRange_14, ownerId_5);
                            }
                        }
                        break;
                    }
                    case aConst::t_CargoHook: {
                        for (auto cpp_range_10 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_10.next(I); ) {
                            Item = pas::construct_call<aItem::TCargoHook>(aItem::TCargoHook_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_6 = OwnerId;
                                std::uint8_t nextRandomIntRange_16 = aMyFunction::NextRandomIntRange(1, InventionLevels[6], RandomState);
                                std::int32_t round_13 = System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_14 = System::Round(static_cast<long double>(aConst::CargoHookBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_17 = aMyFunction::NextRandomIntRange(round_14, round_13, RandomState);
                                aItem::TCargoHook* cpp_arg_14 = pas::checked_cast<aItem::TCargoHook*>(Item);
                                cpp_arg_14->Init(nextRandomIntRange_17, nextRandomIntRange_16, ownerId_6);
                            }
                        }
                        break;
                    }
                    case aConst::t_DefGenerator: {
                        for (auto cpp_range_11 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, 2, RandomState)); cpp_range_11.next(I); ) {
                            Item = pas::construct_call<aItem::TDefGenerator>(aItem::TEquipment_Create);
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                            {
                                aGalaxyStruct::TOwnerId ownerId_7 = OwnerId;
                                std::uint8_t nextRandomIntRange_18 = aMyFunction::NextRandomIntRange(1, InventionLevels[7], RandomState);
                                std::int32_t round_15 = System::Round(static_cast<long double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[1]);
                                std::int32_t round_16 = System::Round(static_cast<long double>(aConst::DefGeneratorBaseSize) * aConst::EquipmentSizeFactors[5]);
                                std::int32_t nextRandomIntRange_19 = aMyFunction::NextRandomIntRange(round_16, round_15, RandomState);
                                aItem::TDefGenerator* cpp_arg_15 = pas::checked_cast<aItem::TDefGenerator*>(Item);
                                cpp_arg_15->Init(nextRandomIntRange_19, nextRandomIntRange_18, ownerId_7);
                            }
                        }
                        break;
                    }
                    case aConst::t_Weapon1: {
                        for (auto cpp_range_12 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(2, InventionLevels[7] + 2, RandomState)); cpp_range_12.next(I); ) {
                            {
                                std::uint8_t cpp_arg_16 = InventionLevels[7];
                                std::uint32_t randomIntRange = aMyFunction::RandomIntRange(1, 100000);
                                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                                WeaponInfo = galaxy->SelectWeaponInfo(randomIntRange, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), cpp_arg_16, 1);
                            }
                            GeneratedWeapon = ([&] {
                                aGalaxyStruct::TOwnerId ownerId_8 = OwnerId;
                                std::int32_t nextRandomIntRange_20 = aMyFunction::NextRandomIntRange(1, InventionLevels[7], RandomState);
                                std::int32_t nextRandomIntRange_21 = ([&] {
                                    std::int32_t round_17 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * aConst::EquipmentSizeFactors[1]);
                                    std::int32_t round_18 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * aConst::EquipmentSizeFactors[5]);
                                    return aMyFunction::NextRandomIntRange(round_18, round_17, RandomState);
                                }());
                                return aItem::CreateGeneratedWeapon(WeaponInfo, nextRandomIntRange_21, nextRandomIntRange_20, ownerId_8);
                            }());
                            pas::list_add(EquipmentShop, reinterpret_cast<void*>(GeneratedWeapon));
                        }
                        break;
                    }
                }
            }
        }
        WaterTiles = System::Round(Graphic->SpaceConfigValues[0]);
        WaterExplored = 0;
        LandTiles = System::Round(Graphic->SpaceConfigValues[1]);
        LandExplored = 0;
        HillTiles = System::Round(Graphic->SpaceConfigValues[2]);
        HillExplored = 0;
        ProbeOrbitCount = System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0));
        if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.5L) {
            ++ProbeOrbitCount;
        }
        if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
            ++ProbeOrbitCount;
        }
        if (OwnerId == aGalaxyStruct::oiUninhabited && GetUnexploredSurfaceTileCount() > 50) {
            for (auto cpp_range_13 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0)), RandomState)); cpp_range_13.next(I); ) {
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                {
                    std::uint32_t cpp_arg_17 = 767 + aMyFunction::NextRandomIntRange(1, 3000, RandomState);
                    aGalaxyStruct::TOwnerId raceToOwner = aConst::RaceToOwner(RaceId);
                    Loot = aItem::CreateRandomLootItem(aItem::ilpTreasure, raceToOwner, cpp_arg_17);
                }
                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Loot)) {
                    static_cast<aTranclucator::TTranclucator*>(artefactTranclucator->Ship)->OwnerShip = nullptr;
                }
                AddSurfaceLootEntry(Loot);
            }
            for (auto cpp_range_14 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(1, System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0, 3.0)), RandomState)); cpp_range_14.next(I); ) {
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L) {
                    continue;
                }
                {
                    std::uint32_t randomState = RandomState;
                    std::uint8_t nextRandomIntRange_22 = aMyFunction::NextRandomIntRange(0, 50, RandomState);
                    aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(nextRandomIntRange_22, 100, randomState, nullptr);
                }
                Module = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                Module->Init(ModuleIndex);
                AddSurfaceLootEntry(Module);
            }
            for (auto cpp_range_15 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 4.0)))); cpp_range_15.next(I); ) {
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.4L) {
                    continue;
                }
                Cistern = pas::construct_call<aItem::TCistern>(aItem::TEquipment_Create);
                Count = aMyFunction::NextRandomIntRange(5, 20, RandomState);
                if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.2L) {
                    Count = aMyFunction::RoundAndTruncateToFives(Count * 2);
                    if (aMyFunction::SeededRandomUnitFloat(RandomState) < 0.1L) {
                        Count = aMyFunction::RoundAndTruncateToFives(Count * 2);
                    }
                }
                {
                    aGalaxyStruct::TOwnerId raceToOwner_2 = aConst::RaceToOwner(RaceId);
                    std::int32_t nextRandomIntRange_23 = aMyFunction::NextRandomIntRange(0, Count, RandomState);
                    Cistern->Init(nextRandomIntRange_23, Count, raceToOwner_2);
                }
                AddSurfaceLootEntry(Cistern);
            }
            MinLevel = 1;
            MaxLevel = 5;
            {
                std::int32_t nextRandomIntRange_24 = aMyFunction::NextRandomIntRange(2, 4, RandomState);
                std::uint32_t& randomState_2 = RandomState;
                WeaponTechLevel = aMyFunction::NextRandomIntRange(1, nextRandomIntRange_24, randomState_2);
            }
            MinSizeFactor = aConst::EquipmentSizeFactors[5];
            MaxSizeFactor = aConst::EquipmentSizeFactors[1];
            for (auto cpp_range_16 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 3.0)))); cpp_range_16.next(I); ) {
                if (I > 1 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.75L) {
                    continue;
                }
                Count = 0;
                Item = nullptr;
                while (true) {
                    ++Count;
                    if (aMyFunction::NextRandomIntRange(1, 130, RandomState) > 70) {
                        WeaponInfo = aGalaxy::Galaxy->SelectWeaponInfo(RandomState, pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}}), std::min<std::int32_t>(WeaponTechLevel + 2, 8), std::max<std::int32_t>(1, WeaponTechLevel - 1));
                        Weight = ([&] {
                            std::int32_t round_19 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * MaxSizeFactor);
                            std::int32_t round_20 = System::Round(static_cast<long double>(WeaponInfo->AverageSize) * MinSizeFactor);
                            return aMyFunction::NextRandomIntRange(round_20, round_19, RandomState);
                        }());
                        Level = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        Item = aItem::CreateGeneratedWeapon(WeaponInfo, Weight, Level, aGalaxyStruct::oiUninhabited);
                    } else {
                        ItemType = static_cast<aConst::TItemType>(aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}})));
                        {
                            std::int32_t round_21 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MaxSizeFactor);
                            std::int32_t round_22 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MinSizeFactor);
                            Weight = aMyFunction::NextRandomIntRange(round_22, round_21, RandomState);
                        }
                        Level = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                        Item = aItem::CreateGeneratedEquipment(ItemType, Weight, Level, aGalaxyStruct::oiUninhabited);
                    }
                    if (Item->Cost < 5000 || Item->Cost < 7.5E+3L && Count > 2 || Item->Cost < 10000 && Count > 3 || Count > 4) {
                        break;
                    }
                    pas::free(Item);
                }
                Item->ConditionPercent = aMyFunction::SeededRandomFloatRange(Item->Id, 1.0E+1, 1.0E+2);
                AddSurfaceLootEntry(Item);
            }
            for (auto cpp_range_17 = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 2.0, 5.0)))); cpp_range_17.next(I); ) {
                if (I > 2 && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.4L) {
                    continue;
                }
                ItemType = static_cast<aConst::TItemType>(aMyFunction::NextRandomIntRange(0, 7, RandomState));
                Count = aMyFunction::NextRandomIntRange(std::max<std::int32_t>(1, aConst::GoodsMarket[ItemType].BaseStock / 20), System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, aConst::GoodsMarket[ItemType].BaseStock / 15, aConst::GoodsMarket[ItemType].BaseStock / 7)), RandomState);
                SavedRandomState = RandomState;
                if (Count < 10) {
                    Part = aMyFunction::NextRandomIntRange(1, 2, RandomState);
                } else {
                    Part = aMyFunction::NextRandomIntRange(1, 4, RandomState);
                }
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(1, Part); cpp_range_18.next(Part); ) {
                    GoodsItem = pas::construct_call<aItem::TGoods>(aItem::TItem_Create);
                    Quantity = pas::idiv(Count, aMyFunction::SeededRandomIntRange(1, 5, RandomState * Part * static_cast<std::uint8_t>(ItemType))) + 1;
                    RandomState = SavedRandomState;
                    GoodsItem->Init(ItemType, Quantity);
                    {
                        std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(2, 5, RandomState * Part * static_cast<std::uint8_t>(ItemType) * 3);
                        GoodsItem->Cost = pas::idiv(GoodsItem->Cost, cpp_right);
                    }
                    AddSurfaceLootEntry(GoodsItem);
                }
            }
            NormalizeSurfaceLootEntries();
        }
        for (auto cpp_range_19 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_19.next(I); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
            pas::list_add(RangerRelations, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(*([&] {
                auto cpp_index = aConst::RaceToOwner(Ranger->PilotRace);
                auto* cpp_array = &aConst::OwnerRelations[aConst::RaceToOwner(RaceId)];
                return &(*cpp_array)[cpp_index];
            }())))));
        }
        HasPlayerLanded = false;
        UpdateOwnerFlags();
    }

    void TPlanet::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::uint8_t Track{};
        std::uint8_t Kind{};
        aShip::TShip* Ship{};
        TSputnik* Satellite{};
        aItem::TItem* Item{};
        PPlanetSurfaceLootEntry Entry{};
        Buffer->AddDWord(Id);
        Buffer->AddIntegerValue(GenerationSeed);
        Buffer->AddDWord(RandomState);
        Buffer->AddWideStringZ(Name);
        Buffer->AddSingle(Orbit.AngleDegrees);
        Buffer->AddSingle(Orbit.Radius);
        std::int32_t i = 4;
        Buffer->AddSingle(OrbitalVelocity);
        Buffer->AddIntegerValue(ReservedSaveValue);
        Buffer->AddIntegerValue(Radius);
        Buffer->AddIntegerValue(WaterTiles);
        Buffer->AddIntegerValue(WaterExplored);
        Buffer->AddIntegerValue(LandTiles);
        Buffer->AddIntegerValue(LandExplored);
        Buffer->AddIntegerValue(HillTiles);
        Buffer->AddIntegerValue(HillExplored);
        Buffer->AddAnsiChar(ProbeOrbitCount);
        Buffer->AddBoolean(HasPlayerLanded);
        for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
            Buffer->AddAnsiChar(InventionLevels[Track]);
        }
        Buffer->AddAnsiChar(CurrentInvention);
        Buffer->AddSingle(CurrentInventionPoints);
        Buffer->AddAnsiChar(ResearchLevelPercent);
        Buffer->AddAnsiChar(ResearchLevelStep);
        Buffer->AddDWord(Population);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Economy));
        Buffer->AddDWord(Money);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(OwnerId));
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(RaceId));
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(Government));
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            Buffer->AddIntegerValue(Goods[Kind].Count);
            Buffer->AddSingle(Goods[Kind].PriceState);
            Buffer->AddIntegerValue(Goods[Kind].PurchasePrice);
            Buffer->AddIntegerValue(Goods[Kind].BaseSalePrice);
            Buffer->AddAnsiChar(GoodsScarcityTicks[Kind]);
            Buffer->AddAnsiChar(GoodsSurplusTicks[Kind]);
        }
        std::int32_t Count = pas::list_count(RangerRelations);
        Buffer->AddWideChar(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, i))));
        }
        Count = pas::list_count(EquipmentShop);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
            Buffer->AddAnsiChar(static_cast<std::uint8_t>(Item->ItemType));
            Item->SaveToBuffer(Buffer);
        }
        Count = 0;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Warriors) - 1); cpp_range_3.next(i); ) {
            Ship = pas::list_at<aShip::TShip>(Warriors, i);
            if (pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                ++Count;
            }
        }
        Buffer->AddWideChar(Count);
        Count = pas::list_count(Warriors);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(i); ) {
            Ship = pas::list_at<aShip::TShip>(Warriors, i);
            if (pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                Buffer->AddAnsiChar(Ship->TypeId);
                Ship->SaveToBuffer(Buffer);
            }
        }
        Buffer->AddWideChar(HomeRangerCount);
        Buffer->AddWideChar(HomeTransportCount);
        Buffer->AddWideChar(u'\000');
        Buffer->AddWideChar(u'\000');
        Buffer->AddWideChar(u'\000');
        Buffer->AddWideChar(GraphicRadius);
        Buffer->AddWideStringZ(GraphName);
        Buffer->AddWideChar(Graphic->RotationTimerInterval);
        Buffer->AddIntegerValue(Graphic->SurfaceMapStep);
        Buffer->AddAnsiChar(Graphic->RingKind);
        Buffer->AddIntegerValue(TextQuestId);
        Count = pas::list_count(Satellites);
        Buffer->AddWideChar(Count);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(i); ) {
            Satellite = pas::list_at<TSputnik>(Satellites, i);
            Satellite->SaveToBuffer(Buffer);
        }
        if (SurfaceLootEntries == nullptr) {
            Buffer->AddWideChar(u'\000');
        } else {
            Buffer->AddWideChar(pas::list_count(SurfaceLootEntries));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range_6.next(i); ) {
                Entry = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
                Buffer->AddAnsiChar(Entry->GridX);
                Buffer->AddAnsiChar(Entry->GridY);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Entry->TerrainKind));
                Buffer->AddIntegerValue(Entry->SurfaceTileIndex);
                Buffer->AddBoolean(Entry->Unavailable);
                Buffer->AddAnsiChar(static_cast<std::uint8_t>(Entry->Item->ItemType));
                Entry->Item->SaveToBuffer(Buffer);
            }
        }
        Buffer->AddBoolean(NoLanding);
        Buffer->AddAnsiChar(static_cast<std::uint8_t>(4 * NoAutomaticShipSpawning + ShopUpdateMode + 8 * NoRandomEvents));
        Buffer->AddBoolean(IsMainPiratePlanet);
        Buffer->AddWideStringZ(CustomFaction);
    }

    void TPlanet_LoadFromBuffer(TPlanet* Self, EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        std::uint8_t Track{};
        aItem::TItem* Item{};
        std::uint8_t Good{};
        std::int32_t i{};
        std::int32_t Count{};
        aShip::TShip* Ship{};
        std::uint8_t ShipType{};
        TSputnik* Satellite{};
        PPlanetSurfaceLootEntry Entry{};
        std::int32_t Stage = 0;
        try {
            Self->Id = EC_Buf::TBufEC_GetUInt32(Buffer);
            if (Galaxy->NextPlanetId <= Self->Id) {
                Galaxy->NextPlanetId = Self->Id + 1;
            }
            Self->GenerationSeed = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->RandomState = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->Name = Buffer->ReadWideString();
            Self->Orbit.AngleDegrees = EC_Buf::TBufEC_GetSingle(Buffer);
            Self->Orbit.Radius = EC_Buf::TBufEC_GetSingle(Buffer);
            Self->OrbitalVelocity = EC_Buf::TBufEC_GetSingle(Buffer);
            Self->ReservedSaveValue = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->Radius = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->WaterTiles = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->WaterExplored = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->LandTiles = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->LandExplored = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->HillTiles = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->HillExplored = EC_Buf::TBufEC_GetInt32(Buffer);
            Self->ProbeOrbitCount = EC_Buf::TBufEC_GetByte(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 99) {
                Self->HasPlayerLanded = EC_Buf::TBufEC_GetBoolean(Buffer);
            } else {
                Self->HasPlayerLanded = false;
            }
            Stage = 1;
            for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
                if (GlobalsV::LoadedSaveVersion <= 90) {
                    EC_Buf::TBufEC_GetByte(Buffer);
                    Self->InventionLevels[Track] = EC_Buf::TBufEC_GetByte(Buffer);
                    if (Track >= 8) {
                        Self->InventionLevels[Track] = std::min<std::int32_t>(8, Self->InventionLevels[Track] * 2 - 1);
                    }
                } else {
                    Self->InventionLevels[Track] = EC_Buf::TBufEC_GetByte(Buffer);
                }
            }
            if (GlobalsV::LoadedSaveVersion <= 90) {
                Self->CurrentInvention = pas::shr(static_cast<std::int32_t>(EC_Buf::TBufEC_GetByte(Buffer)), 1);
            } else {
                Self->CurrentInvention = EC_Buf::TBufEC_GetByte(Buffer);
            }
            Self->CurrentInventionPoints = EC_Buf::TBufEC_GetSingle(Buffer);
            Self->ResearchLevelPercent = EC_Buf::TBufEC_GetByte(Buffer);
            Self->ResearchLevelStep = EC_Buf::TBufEC_GetByte(Buffer);
            Self->Population = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->Economy = static_cast<aGalaxyStruct::TPlanetEconomy>(EC_Buf::TBufEC_GetByte(Buffer));
            Self->Money = EC_Buf::TBufEC_GetUInt32(Buffer);
            Self->OwnerId = static_cast<aGalaxyStruct::TOwnerId>(EC_Buf::TBufEC_GetByte(Buffer));
            Self->RaceId = static_cast<aGalaxyStruct::TOwnerId>(EC_Buf::TBufEC_GetByte(Buffer));
            Self->Government = static_cast<aGalaxyStruct::TPlanetGovernment>(EC_Buf::TBufEC_GetByte(Buffer));
            if (GlobalsV::LoadedSaveVersion < 96) {
                EC_Buf::TBufEC_GetByte(Buffer);
            }
            Stage = 2;
            for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                Self->Goods[Good].Count = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->Goods[Good].PriceState = EC_Buf::TBufEC_GetSingle(Buffer);
                Self->Goods[Good].PurchasePrice = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->Goods[Good].BaseSalePrice = EC_Buf::TBufEC_GetInt32(Buffer);
                Self->GoodsScarcityTicks[Good] = EC_Buf::TBufEC_GetByte(Buffer);
                Self->GoodsSurplusTicks[Good] = EC_Buf::TBufEC_GetByte(Buffer);
            }
            Stage = 3;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
                void* byte = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetByte(Buffer))));
                pas::List* rangerRelations = Self->RangerRelations;
                pas::list_add(rangerRelations, byte);
            }
            Stage = 4;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
                Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
                pas::list_add(Self->EquipmentShop, reinterpret_cast<void*>(Item));
                Item->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 5;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count < 0 || Count > 10000) {
                pas::raise(pas::make_exception<pas::Abort>("Err"_a));
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(i); ) {
                ShipType = EC_Buf::TBufEC_GetByte(Buffer);
                Ship = aShip::CreateShipByType(ShipType);
                pas::list_add(Self->Warriors, reinterpret_cast<void*>(Ship));
                Ship->CurrentStar = Self->CurrentStar;
                Ship->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 6;
            Self->HomeRangerCount = EC_Buf::TBufEC_GetWord(Buffer);
            Self->HomeTransportCount = EC_Buf::TBufEC_GetWord(Buffer);
            EC_Buf::TBufEC_GetWord(Buffer);
            if (GlobalsV::LoadedSaveVersion < 144) {
                EC_Buf::TBufEC_GetWord(Buffer);
            }
            EC_Buf::TBufEC_GetWord(Buffer);
            EC_Buf::TBufEC_GetWord(Buffer);
            Stage = 7;
            Self->GraphicRadius = EC_Buf::TBufEC_GetWord(Buffer);
            Self->GraphName = Buffer->ReadWideString();
            {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Planet"sv, Self->GraphName, ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&Self->Graphic);
                SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
            }
            Self->Graphic->SetPosition(aMyFunction::PolarToPoint(Self->Orbit));
            {
                std::uint32_t word = EC_Buf::TBufEC_GetWord(Buffer);
                SE_Planet::TPlanetSE* graphic_2 = Self->Graphic;
                graphic_2->SetRotationTimerInterval(word);
            }
            Stage = 8;
            {
                std::int32_t int32 = EC_Buf::TBufEC_GetInt32(Buffer);
                SE_Planet::TPlanetSE* graphic_3 = Self->Graphic;
                graphic_3->SetSurfaceMapStep(int32);
            }
            Self->Graphic->OrbitalVelocity = Self->OrbitalVelocity;
            {
                std::uint8_t byte_2 = EC_Buf::TBufEC_GetByte(Buffer);
                SE_Planet::TPlanetSE* graphic_4 = Self->Graphic;
                graphic_4->SetRingKind(byte_2);
            }
            Self->TextQuestId = EC_Buf::TBufEC_GetInt32(Buffer);
            Stage = 9;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(i); ) {
                Satellite = pas::construct_call<TSputnik>(TSputnik_Create);
                Satellite->LoadFromBuffer(Buffer, Galaxy);
                pas::list_add(Self->Satellites, reinterpret_cast<void*>(Satellite));
            }
            Stage = 10;
            Count = EC_Buf::TBufEC_GetWord(Buffer);
            if (Count > 0 && Self->SurfaceLootEntries == nullptr) {
                Self->SurfaceLootEntries = pas::make_object<pas::List>();
            }
            Stage = 11;
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(i); ) {
                pas::get_mem_at(&Entry, static_cast<std::int32_t>(sizeof(TPlanetSurfaceLootEntry)));
                pas::list_add(Self->SurfaceLootEntries, static_cast<void*>(Entry));
                Entry->GridX = EC_Buf::TBufEC_GetByte(Buffer);
                Entry->GridY = EC_Buf::TBufEC_GetByte(Buffer);
                Entry->TerrainKind = static_cast<TPlanetTerrainKind>(EC_Buf::TBufEC_GetByte(Buffer));
                Entry->SurfaceTileIndex = EC_Buf::TBufEC_GetInt32(Buffer);
                Entry->Unavailable = EC_Buf::TBufEC_GetBoolean(Buffer);
                Entry->Item = aItem::CreateItemByType(aItem::MigrateSavedItemType(EC_Buf::TBufEC_GetByte(Buffer)));
                Entry->Item->LoadFromBuffer(Buffer, Galaxy);
            }
            Stage = 12;
            Self->NoLanding = EC_Buf::TBufEC_GetBoolean(Buffer);
            if (GlobalsV::LoadedSaveVersion >= 83) {
                Self->ShopUpdateMode = EC_Buf::TBufEC_GetByte(Buffer);
            } else {
                Self->ShopUpdateMode = 0;
            }
            Self->NoAutomaticShipSpawning = (Self->ShopUpdateMode & 4) > 0;
            Self->NoRandomEvents = (Self->ShopUpdateMode & 8) > 0;
            Self->ShopUpdateMode = Self->ShopUpdateMode & 3;
            Self->IsMainPiratePlanet = EC_Buf::TBufEC_GetBoolean(Buffer);
            if (Self->IsMainPiratePlanet) {
                MainPiratePlanet = Self;
            }
            if (GlobalsV::LoadedSaveVersion >= 166) {
                Self->CustomFaction = Buffer->ReadWideString();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TPlanet.Load, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TPlanet::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t i{};
        pas::WideString Text{};
        aShip::TShip* Ship{};
        PPlanetSurfaceLootEntry Entry{};
        aItem::TItem* Item{};
        fEquipmentShop::TShopSlot* Slot{};
        Block->AddParam(EC_Str::DecodeTextW(u"Pul4awnre2taNgarmEes"_w), Name);
        Block->AddParam(EC_Str::DecodeTextW(u"OpwRn3ewr"_w), aConst::OwnerInfo[OwnerId].InternalName);
        {
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Rja6cEe"_w);
            const pas::WideString& internalName = aConst::OwnerInfo[aConst::RaceToOwner(RaceId)].InternalName;
            Block->AddParam(decodeTextW, internalName);
        }
        Block->AddParam(EC_Str::DecodeTextW(u"Elc0o5neowmWyq"_w), aConst::PlanetEconomyInfo[Economy].InternalName);
        Block->AddParam(EC_Str::DecodeTextW(u"GLotvUecrBmnemn7t"_w), aConst::PlanetGovernmentMarket[Government].InternalName);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Radius);
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"ItSaiNzze"_w);
            Block->AddParam(decodeTextW_2, intToStr);
        }
        {
            const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Orbit.Radius), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"OcrublietyRnakdlipuns"_w);
            Block->AddParam(decodeTextW_3, cpp_arg);
        }
        {
            const pas::WideString& cpp_arg_2 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(Orbit.AngleDegrees), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"OsrabniktuAinegilne"_w);
            Block->AddParam(decodeTextW_4, cpp_arg_2);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, 0)))));
            const pas::WideString& decodeTextW_5 = EC_Str::DecodeTextW(u"Rpe7lyamtgi4oendThokP4lWasyfeKry"_w);
            Block->AddParam(decodeTextW_5, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(InventionLevels[7]));
            const pas::WideString& decodeTextW_6 = EC_Str::DecodeTextW(u"IsMraliunaTrepcohaLienvuelle"_w);
            Block->AddParam(decodeTextW_6, intToStr_3);
        }
        Text = pas::wide_int_to_str(static_cast<std::int32_t>(InventionLevels[0]));
        for (i = 1; i <= 19; ++i) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(static_cast<std::int32_t>(InventionLevels[static_cast<std::uint8_t>(i)]))});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"Toe5cfh2LSexvNejlusw"_w), Text);
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(static_cast<std::int32_t>(CurrentInvention));
            const pas::WideString& decodeTextW_7 = EC_Str::DecodeTextW(u"C2u4rrrTeengtyIwnsvgeEn6tjieodn"_w);
            Block->AddParam(decodeTextW_7, intToStr_4);
        }
        {
            const pas::WideString& cpp_arg_3 = static_cast<pas::WideString>(pas::float_to_str(static_cast<pas::Extended>(CurrentInventionPoints), SysUtils::DecimalSeparator));
            const pas::WideString& decodeTextW_8 = EC_Str::DecodeTextW(u"CluurtreewnstQIvnhv6eenwtfijo6ntPwoSirn5tts7"_w);
            Block->AddParam(decodeTextW_8, cpp_arg_3);
        }
        // 'EqShop'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->AddBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_w));
            if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(i); ) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->SaveToBlock(cpp_with->AddBlockByPath(Text));
                }
            } else if (aPlayer::GetPlayer()->CurrentPlanet == this) {
                if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_2.next(i); ) {
                        Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, i);
                        Item = Slot->Item;
                        if (Item != nullptr) {
                            Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                            Item->SaveToBlock(cpp_with->AddBlockByPath(Text));
                        }
                    }
                }
            }
            cpp_with->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
        }
        Text = pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[0]].Count);
        for (i = 1; i <= 7; ++i) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].Count)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesa"_w), Text);
        Text = pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[0]].PurchasePrice);
        for (i = 1; i <= 7; ++i) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].PurchasePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_w), Text);
        Text = pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[0]].BaseSalePrice);
        for (i = 1; i <= 7; ++i) {
            Text = pas::concat_wide({Text, u",", pas::wide_int_to_str(Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].BaseSalePrice)});
        }
        Block->AddParam(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_w), Text);
        // 'Garrison'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"GlamrirLihsaoln"_w));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Warriors) - 1); cpp_range_3.next(i); ) {
                Ship = pas::list_at<aShip::TShip>(Warriors, i);
                if (pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Ship->Id))), EC_Str::DecodeTextW(u"WfajrRrkiSo4rgImd5"_w)});
                    Ship->SaveToBlock(cpp_with_2->AddBlockByPath(Text));
                }
            }
        }
        {
            const pas::WideString& intToStr_5 = pas::wide_int_to_str(WaterTiles);
            const pas::WideString& decodeTextW_9 = EC_Str::DecodeTextW(u"WuartTewrfSgpwaQcde"_w);
            Block->AddParam(decodeTextW_9, intToStr_5);
        }
        {
            const pas::WideString& intToStr_6 = pas::wide_int_to_str(WaterExplored);
            const pas::WideString& decodeTextW_10 = EC_Str::DecodeTextW(u"WbantderrwCSofmgpUlkaltwef"_w);
            Block->AddParam(decodeTextW_10, intToStr_6);
        }
        {
            const pas::WideString& intToStr_7 = pas::wide_int_to_str(LandTiles);
            const pas::WideString& decodeTextW_11 = EC_Str::DecodeTextW(u"LLagnsd3SwpFascge4"_w);
            Block->AddParam(decodeTextW_11, intToStr_7);
        }
        {
            const pas::WideString& intToStr_8 = pas::wide_int_to_str(LandExplored);
            const pas::WideString& decodeTextW_12 = EC_Str::DecodeTextW(u"LgaEnwdsCfogmHpjlya5tre"_w);
            Block->AddParam(decodeTextW_12, intToStr_8);
        }
        {
            const pas::WideString& intToStr_9 = pas::wide_int_to_str(HillTiles);
            const pas::WideString& decodeTextW_13 = EC_Str::DecodeTextW(u"HbiFldleSrptaycue"_w);
            Block->AddParam(decodeTextW_13, intToStr_9);
        }
        {
            const pas::WideString& intToStr_10 = pas::wide_int_to_str(HillExplored);
            const pas::WideString& decodeTextW_14 = EC_Str::DecodeTextW(u"HninlglnCfodmFpflFastee"_w);
            Block->AddParam(decodeTextW_14, intToStr_10);
        }
        {
            const pas::WideString& intToStr_11 = pas::wide_int_to_str(static_cast<std::int32_t>(ProbeOrbitCount));
            const pas::WideString& decodeTextW_15 = EC_Str::DecodeTextW(u"OyrebwiftlCknstx"_w);
            Block->AddParam(decodeTextW_15, intToStr_11);
        }
        // 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with_3 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"Sataokrgalgae"_w));
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_4.next(i); ) {
                if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, i)->LocationOwner == this) {
                    Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, i)->Item;
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->SaveToBlock(cpp_with_3->AddBlockByPath(Text));
                }
            }
            cpp_with_3->AddParam(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w), u""_wref.get());
        }
        // 'Treasure'
        {
            EC_BlockPar::TBlockParEC* cpp_with_4 = Block->AddBlockByPath(EC_Str::DecodeTextW(u"TurieKalsauOrden"_w));
            if (SurfaceLootEntries != nullptr) {
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range_5.next(i); ) {
                    Entry = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
                    Text = pas::concat_wide_reverse({pas::wide_int_to_str(i + 1), EC_Str::DecodeTextW(u"HyiIdedfehnjIytrewm"_w)});
                    {
                        EC_BlockPar::TBlockParEC* cpp_with_5 = cpp_with_4->AddBlockByPath(Text);
                        {
                            const pas::WideString& intToStr_12 = pas::wide_int_to_str(static_cast<std::int32_t>(Entry->TerrainKind));
                            const pas::WideString& decodeTextW_16 = EC_Str::DecodeTextW(u"LaawnedrTtyhpuei"_w);
                            cpp_with_5->AddParam(decodeTextW_16, intToStr_12);
                        }
                        {
                            const pas::WideString& intToStr_13 = pas::wide_int_to_str(Entry->SurfaceTileIndex);
                            const pas::WideString& decodeTextW_17 = EC_Str::DecodeTextW(u"DjetpEtwh"_w);
                            cpp_with_5->AddParam(decodeTextW_17, intToStr_13);
                        }
                        {
                            EC_BlockPar::TBlockParEC* addBlockByPath = cpp_with_5->AddBlockByPath(pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Entry->Item->Id))), EC_Str::DecodeTextW(u"IrtteEmtIIdy"_w)}));
                            aItem::TItem* item = Entry->Item;
                            item->SaveToBlock(addBlockByPath);
                        }
                    }
                }
            }
            cpp_with_4->AddParam(EC_Str::DecodeTextW(u"Cur5erawtre3NregwgHjikdHdgern4IFthejm6"_w), u""_wref.get());
        }
        Block->AddParam(EC_Str::DecodeTextW(u"CorFedaWtaesNfeTwgShhji6pw"_w), u""_wref.get());
    }

    // Loads editable text fields, updates existing items/ships and processes creation requests. The first matching item name ends the search even when its type is disallowed or creation returns nil.
    void TPlanet::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t i{};
        pas::WideString Text{};
        pas::WideString Part{};
        pas::WideString ShipName{};
        aShip::TShip* Ship{};
        PPlanetSurfaceLootEntry Entry{};
        aItem::TItem* Item{};
        aConst::TItemType ItemType{};
        std::uint8_t ShipType{};
        aPlayer::PStorageEntry Storage{};
        fEquipmentShop::TShopSlot* Slot{};
        aGalaxyStruct::TOwnerId OldOwner{};
        aGalaxyStruct::TOwnerId Owner{};
        aGalaxyStruct::TOwnerId OldRace{};
        aGalaxyStruct::TDominatorSeries OldSeries{};
        aGalaxyStruct::TDominatorSeries Series{};
        Name = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Pul4awnre2taNgarmEes"_w)));
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"OpwRn3ewr"_w)));
        for (i = 0; i <= 7; ++i) {
            if (Text == aConst::OwnerInfo[static_cast<aGalaxyStruct::TOwnerId>(i)].InternalName) {
                OwnerId = static_cast<aGalaxyStruct::TOwnerId>(i);
            }
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Rja6cEe"_w)));
        for (i = 0; i <= 4; ++i) {
            if (Text == aConst::OwnerInfo[static_cast<aGalaxyStruct::TOwnerId>(i)].InternalName) {
                RaceId = static_cast<aGalaxyStruct::TOwnerId>(i);
            }
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Elc0o5neowmWyq"_w)));
        for (i = 0; i <= 2; ++i) {
            if (Text == aConst::PlanetEconomyInfo[static_cast<aGalaxyStruct::TPlanetEconomy>(i)].InternalName) {
                Economy = static_cast<aGalaxyStruct::TPlanetEconomy>(i);
            }
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"GLotvUecrBmnemn7t"_w)));
        for (i = 0; i <= 4; ++i) {
            if (Text == aConst::PlanetGovernmentMarket[static_cast<aGalaxyStruct::TPlanetGovernment>(i)].InternalName) {
                Government = static_cast<aGalaxyStruct::TPlanetGovernment>(i);
            }
        }
        Orbit.Radius = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"OcrublietyRnakdlipuns"_w))));
        Orbit.AngleDegrees = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"OsrabniktuAinegilne"_w))));
        pas::list_put(RangerRelations, 0, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Rpe7lyamtgi4oendThokP4lWasyfeKry"_w)))))))));
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordesa"_w)));
        for (i = 0; i <= 7; ++i) {
            Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv)));
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordesaSrakloe"_w)));
        for (i = 0; i <= 7; ++i) {
            Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].PurchasePrice = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv)));
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"SihrolpaGloiordesaBruhy"_w)));
        for (i = 0; i <= 7; ++i) {
            Goods[aConst::GoodsTextOrder[static_cast<std::uint8_t>(i)]].BaseSalePrice = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv)));
        }
        // 'EqShop'
        {
            EC_BlockPar::TBlockParEC* cpp_with = Block->GetBlockByPath(EC_Str::DecodeTextW(u"EdqeSahloEp"_w));
            if (EquipmentShop != nullptr && pas::list_count(EquipmentShop) > 0) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(i); ) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->virtual_TItem_LoadFromBlock(cpp_with->GetBlockByPath(Text));
                }
            } else if (aPlayer::GetPlayer()->CurrentPlanet == this) {
                if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_2.next(i); ) {
                        Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, i);
                        Item = Slot->Item;
                        if (Item != nullptr) {
                            Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                            Item->virtual_TItem_LoadFromBlock(cpp_with->GetBlockByPath(Text));
                        }
                    }
                }
            }
            Text = cpp_with->GetParam(pas::view(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w)));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range_3.next(i); ) {
                Part = EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv);
                for (auto cpp_range_4 = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_UselessCountableItem); cpp_range_4.next(ItemType); ) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                            Item = aItem::CreateDefaultItemByType(ItemType);
                            if (Item != nullptr) {
                                if (fEquipmentShop::TemporaryShopSlots != nullptr && fEquipmentShop::TemporaryShopPlanet == this) {
                                    fEquipmentShop::RestoreTemporaryShopStock();
                                    pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                                    fEquipmentShop::BuildTemporaryShopSlotGrid();
                                } else {
                                    pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
        // 'Storage'
        {
            EC_BlockPar::TBlockParEC* cpp_with_2 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"Sataokrgalgae"_w));
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_5.next(i); ) {
                if (pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, i)->LocationOwner == this) {
                    Item = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, i)->Item;
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Item->Id))), EC_Str::DecodeTextW(u"ImtreamrIodo"_w)});
                    Item->virtual_TItem_LoadFromBlock(cpp_with_2->GetBlockByPath(Text));
                }
            }
            Text = cpp_with_2->GetParam(pas::view(EC_Str::DecodeTextW(u"AodEdrIstaelma"_w)));
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range_6.next(i); ) {
                Part = EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv);
                for (auto cpp_range_7 = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_UselessCountableItem); cpp_range_7.next(ItemType); ) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_ArtefactHull), static_cast<std::int32_t>(aConst::t_ArtFastRacks)) || pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Protoplasm), static_cast<std::int32_t>(aConst::t_Satellite))) {
                            Item = aItem::CreateDefaultItemByType(ItemType);
                            if (Item != nullptr) {
                                pas::get_mem_at(&Storage, static_cast<std::int32_t>(sizeof(aPlayer::TStorageEntry)));
                                Storage->LocationOwner = this;
                                Storage->SlotIndex = aPlayer::GetPlayer()->FindNextStorageSlot(this);
                                Storage->Item = Item;
                                pas::list_add(aPlayer::GetPlayer()->StorageEntries, static_cast<void*>(Storage));
                                aPlayer::GetPlayer()->RefreshStorageBubbles();
                            }
                        }
                        break;
                    }
                }
            }
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Toe5cfh2LSexvNejlusw"_w)));
        for (i = 0; i <= 19; ++i) {
            InventionLevels[static_cast<std::uint8_t>(i)] = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv)));
        }
        CurrentInvention = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"C2u4rrrTeengtyIwnsvgeEn6tjieodn"_w)))));
        CurrentInventionPoints = EC_Str::ExtractDecimalToSingleW(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"CluurtreewnstQIvnhv6eenwtfijo6ntPwoSirn5tts7"_w))));
        // 'Garrison'
        {
            EC_BlockPar::TBlockParEC* cpp_with_3 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"GlamrirLihsaoln"_w));
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Warriors) - 1); cpp_range_8.next(i); ) {
                Ship = pas::list_at<aShip::TShip>(Warriors, i);
                if (pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                    Text = pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Ship->Id))), EC_Str::DecodeTextW(u"WfajrRrkiSo4rgImd5"_w)});
                    Ship->LoadFromBlock(cpp_with_3->GetBlockByPath(Text));
                }
            }
        }
        WaterTiles = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"WuartTewrfSgpwaQcde"_w)))));
        WaterExplored = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"WbantderrwCSofmgpUlkaltwef"_w)))));
        LandTiles = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"LLagnsd3SwpFascge4"_w)))));
        LandExplored = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"LgaEnwdsCfogmHpjlya5tre"_w)))));
        HillTiles = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"HbiFldleSrptaycue"_w)))));
        HillExplored = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"HninlglnCfodmFpflFastee"_w)))));
        ProbeOrbitCount = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"OyrebwiftlCknstx"_w)))));
        // 'Treasure'
        {
            EC_BlockPar::TBlockParEC* cpp_with_4 = Block->GetBlockByPath(EC_Str::DecodeTextW(u"TurieKalsauOrden"_w));
            if (SurfaceLootEntries != nullptr) {
                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range_9.next(i); ) {
                    Entry = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
                    Text = pas::concat_wide_reverse({pas::wide_int_to_str(i + 1), EC_Str::DecodeTextW(u"HyiIdedfehnjIytrewm"_w)});
                    {
                        EC_BlockPar::TBlockParEC* cpp_with_5 = cpp_with_4->GetBlockByPath(Text);
                        Entry->TerrainKind = static_cast<TPlanetTerrainKind>(SysUtils::StrToInt(static_cast<pas::AnsiString>(cpp_with_5->GetParam(pas::view(EC_Str::DecodeTextW(u"LaawnedrTtyhpuei"_w))))));
                        Entry->SurfaceTileIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(cpp_with_5->GetParam(pas::view(EC_Str::DecodeTextW(u"DjetpEtwh"_w)))));
                        {
                            EC_BlockPar::TBlockParEC* blockByPath = cpp_with_5->GetBlockByPath(pas::concat_wide_reverse({pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Entry->Item->Id))), EC_Str::DecodeTextW(u"IrtteEmtIIdy"_w)}));
                            aItem::TItem* item = Entry->Item;
                            item->virtual_TItem_LoadFromBlock(blockByPath);
                        }
                    }
                }
            }
            Text = cpp_with_4->GetParam(pas::view(EC_Str::DecodeTextW(u"Cur5erawtre3NregwgHjikdHdgern4IFthejm6"_w)));
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range_10.next(i); ) {
                Part = EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv);
                for (auto cpp_range_11 = pas::for_to<aConst::TItemType>(aConst::t_Food, aConst::t_UselessCountableItem); cpp_range_11.next(ItemType); ) {
                    if (aConst::ItemTypeNames[ItemType] == Part) {
                        if (pas::in_set<aConst::t_Food, aConst::t_Narcotics, aConst::t_ArtefactHull, aConst::t_Satellite>(ItemType)) {
                            if (ItemType != aConst::t_Hull) {
                                Item = aItem::CreateDefaultItemByType(ItemType);
                                if (Item != nullptr) {
                                    AddSurfaceLootEntry(Item);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
        Text = Block->GetParam(pas::view(EC_Str::DecodeTextW(u"CorFedaWtaesNfeTwgShhji6pw"_w)));
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1); cpp_range_12.next(i); ) {
            Part = EC_Str::ExtractDelimitedPartW(pas::view(Text), i, u","sv);
            ShipName = EC_Str::ExtractDelimitedPartW(pas::view(Part), 0, u"."sv);
            OldOwner = OwnerId;
            OldRace = RaceId;
            OldSeries = CurrentStar->Status.DominatorSeries;
            if (EC_Str::CountDelimitedPartsW(pas::view(Part), u"."sv) > 1) {
                Part = EC_Str::ExtractDelimitedPartW(pas::view(Part), 1, u"."sv);
                for (auto cpp_range_13 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_13.next(Series); ) {
                    if (aConst::DominatorSeriesNames[Series] == Part) {
                        CurrentStar->Status.DominatorSeries = Series;
                    }
                }
                for (auto cpp_range_14 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_14.next(Owner); ) {
                    if (aConst::OwnerInfo[Owner].InternalName == Part) {
                        OwnerId = Owner;
                        if (pas::contains(aConst::PlanetOwnerMasks.Coalition, Owner)) {
                            RaceId = aConst::OwnerToRace(Owner);
                        }
                    }
                }
            }
            for (ShipType = static_cast<std::uint8_t>(0); ShipType <= static_cast<std::uint8_t>(13); ++ShipType) {
                if (aConst::ShipTypeNames[ShipType].Name == ShipName) {
                    switch (ShipType) {
                        case 0: SpawnWeightedDominatorShip(); break;
                        case 1: BuyRanger(100); break;
                        case 2: SpawnTransport(0, 100); break;
                        case 3: BuyPirate(100); break;
                        case 4: BuyWarrior(100); break;
                        case 5: SpawnTranclucator(true); break;
                    }
                    break;
                }
            }
            CurrentStar->Status.DominatorSeries = OldSeries;
            OwnerId = OldOwner;
            RaceId = OldRace;
        }
    }

    // Forwards Galaxy from TStar.ResolveLoadedReferences to shop, garrison and surface item resolvers; removes incompatible main-pirate-planet modules from saves older than 106.
    void TPlanet::ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip* Ship{};
        std::int32_t i{};
        aItem::TItem* Item{};
        PPlanetSurfaceLootEntry Entry{};
        std::int32_t Count = pas::list_count(EquipmentShop);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(i); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
            Item->ResolveLoadedReferences(Galaxy);
        }
        if (GlobalsV::LoadedSaveVersion < 106 && IsMainPiratePlanet) {
            const std::int32_t cpp_first = Count - 1;
            if (cpp_first >= 0) {
                for (i = cpp_first; i >= 0; --i) {
                    Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
                    if (aItem::TEquipment* equipment = pas::class_cast_if<aItem::TEquipment*>(Item); equipment != nullptr && equipment->SpecialModuleIndex > 0 && static_cast<std::uint8_t>(pas::contains(aConst::MicroModuleTemplates[equipment->SpecialModuleIndex - 1].OfferStationTypes, aGalaxyStruct::rstPirateBase) ^ 1)) {
                        pas::list_delete(EquipmentShop, i);
                        pas::free(Item);
                    }
                }
            }
        }
        Count = pas::list_count(Warriors);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(i); ) {
            Ship = pas::list_at<aShip::TShip>(Warriors, i);
            Ship->virtual_TShip_ResolveLoadedReferences(Galaxy);
        }
        if (SurfaceLootEntries != nullptr) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range_3.next(i); ) {
                Entry = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
                Entry->Item->ResolveLoadedReferences(Galaxy);
            }
        }
        UpdateOwnerFlags();
    }

    // Daily strength, control and delay gates; returns nil when no ship is spawned.
    void* TPlanet::TrySpawnDominator() {
        std::int32_t i{};
        aGalaxy::TStar* Star{};
        void* Result = nullptr;
        if (CurrentStar->Status.DominatorSeries == aGalaxyStruct::dsTerron) {
            if (aKling::TerronShip == nullptr || aGalaxy::Galaxy->TerronGrowLockTurn != 0 || aGalaxy::Galaxy->TerronToStarTurn >= 1073741824) {
                return Result;
            }
        } else if (CurrentStar->Status.DominatorSeries == aGalaxyStruct::dsBlazer) {
            if (aKling::BlazerShip == nullptr || aGalaxy::Galaxy->BlazerLandingPlanetId != 0 || aKling::BlazerShip->DestroyQueued) {
                return Result;
            }
        }
        std::uint8_t Control = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
        std::int32_t SeriesStars = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(i); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, i);
            if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators && CurrentStar->Status.DominatorSeries == Star->Status.DominatorSeries && Star->Status.CustomFaction == u"") {
                ++SeriesStars;
            }
        }
        std::uint8_t SeriesControl = System::Round(pas::real_divide(SeriesStars, pas::list_count(aGalaxy::Galaxy->Stars)) * 1.0E+2L);
        std::int32_t MaximumShips = 10;
        switch (CurrentStar->Status.DominatorSeries) {
            case aGalaxyStruct::dsKeller: MaximumShips = 15; break;
            case aGalaxyStruct::dsBlazer: MaximumShips = 13; break;
            case aGalaxyStruct::dsTerron: MaximumShips = 11; break;
        }
        std::int32_t Jitter = aMyFunction::SeededRandomIntRange(0, 5, GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 500);
        MaximumShips = ([&] {
            std::int64_t cpp_left_2 = System::Round(aMyFunction::RemapClamped(SeriesControl, 0.0, 33.0, MaximumShips, 1.0E+1)) + Jitter;
            std::int64_t cpp_left = cpp_left_2 - System::Round(aMyFunction::RemapClamped(SeriesControl, 34.0, 1.0E+2, 0.0, 3.0));
            return cpp_left + System::Round(aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetDominatorSeriesControlShare(CurrentStar->Status.DominatorSeries), 0.0, 1.0, 3.0, 0.0));
        }());
        std::int32_t Chance = 0;
        switch (CurrentStar->Status.DominatorSeries) {
            case aGalaxyStruct::dsKeller: Chance = 20; break;
            case aGalaxyStruct::dsBlazer: Chance = 40; break;
            case aGalaxyStruct::dsTerron: Chance = 60; break;
        }
        std::uint8_t ControlThreshold = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->CurrentTurn, 3.0E+2, 1.125E+4, 4.0E+1, 8.0E+1));
        {
            pas::Extended cpp_left_3 = aMyFunction::RemapClamped(Control, 0.0, ControlThreshold, Chance * 0.5L, 0.0);
            Chance = System::Round(cpp_left_3 + aMyFunction::RemapClamped(Control, ControlThreshold, 1.0E+2, Chance * 0.5L, 1.0));
        }
        if (CurrentStar->Status.Battle != 0) {
            Chance += 10;
        }
        std::int32_t BaseDelay = System::Round(aGalaxy::Galaxy->ScaleDifficultyExponentially(aGalaxy::Galaxy->GetDominatorSpawnLevel(), 81.0f, 0.333f));
        pas::Extended cpp_left_4 = aMyFunction::RemapClamped(Control, 0.0, ControlThreshold, 1.0, BaseDelay * 0.5L);
        std::int32_t Delay = System::Round(cpp_left_4 + aMyFunction::RemapClamped(Control, ControlThreshold, 1.0E+2, 0.0, BaseDelay * 0.5L));
        if (aGalaxy::Galaxy->CurrentTurn >= 666) {
            if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                Chance = 70;
                Delay = 3;
                MaximumShips = 15;
            } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                Chance = 85;
                Delay = 2;
                MaximumShips = 15;
            } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                Chance = 100;
                Delay = 1;
                MaximumShips = 15;
            }
        }
        std::int32_t ShipCount = CurrentStar->CountStandardDominatorsOfLocalSeries();
        if (ShipCount == 0) {
            Chance = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(Chance * 0.2L));
        }
        if (CurrentStar->DaysSinceLastNpcShipSpawn > Delay && aMyFunction::NextRandomIntRange(1, 100, RandomState) <= Chance && ShipCount < MaximumShips) {
            return SpawnWeightedDominatorShip();
        }
        return Result;
    }

    void TPlanet_NextDay(TPlanet* Self) {
        std::uint8_t Good{};
        std::int32_t N{};
        pas::Extended Strength{};
        aShip::TShip* Ship{};
        double PirateSpawnFactor{};
        double GarrisonSpawnFactor{};
        double PirateLimitFactor{};
        double GarrisonLimitFactor{};
        double Boost{};
        double Budget{};
        double SystemRatio{};
        std::int32_t PirateKills{};
        std::int32_t ClanShips{};
        std::int32_t IndependentShips{};
        aGalaxy::TStar* NearbyStar{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        std::int32_t I = 4;
        Self->Orbit.AngleDegrees = aMyFunction::WrapHeadingDegrees(Self->Orbit.AngleDegrees);
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        if (Self->OwnerId != aGalaxyStruct::oiUninhabited && Self->CurrentStar->Status.CustomFaction != u"") {
            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.7L) {
                Self->AdvanceInventionProgress();
            }
            return;
        }
        try {
            {
                aGalaxyStruct::TOwnerId cpp_case = Self->OwnerId;
                if (cpp_case >= aGalaxyStruct::oiMaloc && cpp_case <= aGalaxyStruct::oiGaal) {
                    // Native growth adds 300 even when already above the radius-based population.
                    if (Self->CalculateBasePopulation() < Self->Population) {
                        Self->Population += 300;
                    } else {
                        Self->Population += System::Trunc(Self->Population * 0.02L);
                    }
                    Self->TryTriggerEconomicEvent();
                    aPlanet::TPlanet_UpdateMarketState(Self);
                    Self->Money += System::Trunc(Self->Population * 0.001L);
                    if (static_cast<std::uint8_t>(Self->IsMainPiratePlanet ^ 1) && static_cast<std::uint8_t>(Self->NoAutomaticShipSpawning ^ 1)) {
                        if (Self->CurrentStar->Constellation->Id != 20) {
                            if (([&] {
                                pas::Extended real_min = pas::real_min<pas::Extended>(aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 1.5L, 63.0L);
                                return aGalaxy::Galaxy->CountEligibleRangers() < real_min + aGalaxy::Galaxy->GetExtraRangerCount();
                            }()) && Self->CurrentStar->CountEligibleRangersInSpace() < aGalaxy::Galaxy->GetExtraRangerCount() + 1 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.04L) {
                                Self->BuyRanger(100);
                            }
                            if (Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stTransport] < 5 && ([&] {
                                std::int32_t cpp_left_4 = aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) * 3;
                                return cpp_left_4 + aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 9;
                            }()) > aGalaxy::Galaxy->TransportCount && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L && Self->HomeTransportCount < 2) {
                                Self->SpawnTransport(0, 100);
                            }
                            if (Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stPirate] < 2 && aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) > aGalaxy::Galaxy->PirateCount) {
                                pas::Extended cpp_left_5 = aMyFunction::NextRandomUnitFloat(Self->RandomState);
                                if (cpp_left_5 < pas::sqr(pas::real_divide(aConst::PlanetRaceMarket[Self->RaceId].PirateRelationFactor, 1.0E+1L))) {
                                    Self->BuyPirate(100);
                                }
                            }
                        }
                        if (aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) > 1) {
                            N = 1;
                        } else {
                            N = 2;
                        }
                        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.01L * N) {
                            if (pas::list_count(Self->Warriors) < static_cast<long double>(aMyFunction::RemapClamped(Self->Radius, 6.0E+1, 1.0E+2, 1.0, 3.0)) * N) {
                                Self->BuyWarrior(100);
                            } else if (aGalaxy::Galaxy->RangerSpawnQuotas[Self->RaceId] > 0 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.1L) {
                                Self->BuyFlagship(200);
                            }
                        }
                    }
                    Self->AdvanceInventionProgress();
                    Self->RefreshEquipmentShopInventory();
                    if (Self->HasHostileShipsInSystem()) {
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Self->Warriors) - 1); cpp_range_7.next(I); ) {
                            Ship = pas::list_at<aShip::TShip>(Self->Warriors, I);
                            if (Ship->CurrentPlanet == Self) {
                                if (pas::list_indexof(Self->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) == -1) {
                                    pas::list_add(Self->CurrentStar->Ships, reinterpret_cast<void*>(Ship));
                                }
                                if (!Ship->RepairHullAtLocation()) {
                                    Ship->OrderTakeoff();
                                }
                            }
                        }
                    } else {
                        const std::int32_t cpp_first = pas::list_count(Self->Warriors) - 1;
                        if (cpp_first >= 0) {
                            for (I = cpp_first; I >= 0; --I) {
                                Ship = pas::list_at<aShip::TShip>(Self->Warriors, I);
                                if (Ship->ScriptShip == nullptr && Ship->LiberationGroup == nullptr && Ship->CurrentPlanet == Self) {
                                    N = pas::list_indexof(Self->CurrentStar->Ships, reinterpret_cast<void*>(Ship));
                                    if (N >= 0) {
                                        pas::list_delete(Self->CurrentStar->Ships, N);
                                        Ship->EnemyShip = nullptr;
                                        Ship->TruceShip = nullptr;
                                        Ship->PartnerShip = nullptr;
                                        Ship->GetHull()->HullPoints = Ship->GetHull()->Weight;
                                    } else {
                                        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                                            aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                        }
                                        if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.02L) {
                                            Ship->RefreshDerivedStats(true);
                                            if (Ship->Wealth < aGalaxy::Galaxy->MaxRangerWealth * 0.3L || Ship->StrengthInAverageRanger < 0.7L) {
                                                if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.8L) {
                                                    Ship->SetMoney(Ship->Money + std::max<std::int32_t>(1000, std::min<std::int32_t>(5000, aGalaxy::Galaxy->MaxRangerWealth / 15)));
                                                } else {
                                                    Ship->SetMoney(Ship->Money + std::max<std::int32_t>(2000, std::min<std::int32_t>(10000, aGalaxy::Galaxy->MaxRangerWealth / 7)));
                                                }
                                            }
                                            if (aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= 5 && Ship->Wealth < aGalaxy::Galaxy->MaxRangerWealth * 0.6L) {
                                                Ship->SetMoney(Ship->Money + std::max<std::int32_t>(3000, std::min<std::int32_t>(15000, aGalaxy::Galaxy->MaxRangerWealth / 7)));
                                            }
                                            Ship->RestoreEssentialEquipment();
                                            if (reinterpret_cast<aWarrior::TWarrior*>(Ship)->WarriorType != aWarrior::wtFlagship && (Ship->StrengthInBestRanger < 0.2L || aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.1L)) {
                                                aShip::TShip_GenerateExtraWeapon(Ship);
                                            }
                                            if (Ship->StrengthInBestRanger < 0.5L && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                                                Ship->ImproveRandomEquipment(true);
                                            }
                                            if (Ship->StrengthInBestRanger < 0.3L && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.1L) {
                                                Ship->GainExperience(aMyFunction::NextRandomIntRange(500, 1500, Self->RandomState), 0);
                                                pas::checked_cast<aWarrior::TWarrior*>(Ship)->TrainSkillsAutomatically();
                                            }
                                            Ship->RefreshDerivedStats(true);
                                        }
                                    }
                                    if (aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= 5 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L) {
                                        Ship->ImproveRandomEquipment(true);
                                    }
                                    if (aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) <= 2 && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L) {
                                        aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                        Ship->RestoreEssentialEquipment();
                                        Ship->ImproveRandomEquipment(true);
                                    }
                                    if (reinterpret_cast<aWarrior::TWarrior*>(Ship)->IsHomePatrolTurn()) {
                                        aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                        aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                        pas::list_add(Self->CurrentStar->Ships, reinterpret_cast<void*>(Ship));
                                        Ship->OrderTakeoff();
                                        if (reinterpret_cast<aWarrior::TWarrior*>(Ship)->WarriorType == aWarrior::wtFlagship) {
                                            reinterpret_cast<aWarrior::TWarrior*>(Ship)->ReassignFlagshipHomePlanet();
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (cpp_case == aGalaxyStruct::oiDominator) {
                    for (Good = static_cast<std::uint8_t>(0); Good <= static_cast<std::uint8_t>(7); ++Good) {
                        Self->Goods[Good].Count = 0;
                    }
                    Self->Money = 0;
                    if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.7L) {
                        Self->AdvanceInventionProgress();
                    }
                    if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                        Self->RefreshEquipmentShopInventory();
                    }
                    if (!Self->NoAutomaticShipSpawning) {
                        Self->TrySpawnDominator();
                    }
                    if (Self->HasHostileShipsInSystem()) {
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_6.next(I); ) {
                            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                            if (Ship->CurrentPlanet == Self) {
                                Ship->OrderTakeoff();
                            }
                        }
                    }
                } else if (cpp_case == aGalaxyStruct::oiUninhabited) {
                    Self->TryResetSurfaceLootAfterLongAbsence();
                } else if (cpp_case == aGalaxyStruct::oiPirate) {
                    if (Self->IsMainPiratePlanet && aGalaxy::Galaxy->PirateWinType != 3) {
                        if (([&] {
                            aGalaxyStruct::TPercent cpp_left = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates);
                            return cpp_left > static_cast<std::uint32_t>(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) * 2);
                        }()) && aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates) > 10 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                                Ship = pas::list_at<aShip::TShip>(aGalaxy::Galaxy->Rangers, I);
                                if (static_cast<std::uint8_t>(reinterpret_cast<aRanger::TRanger*>(Ship)->ExcludedFromRating ^ 1) && Ship->OwnerId != aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(aShip::TShip_IsInPrison(Ship) ^ 1)) {
                                    Self->ChangeRelationToRanger(Ship, -1);
                                }
                            }
                        }
                        if (([&] {
                            aGalaxyStruct::TPercent cpp_left_2 = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates);
                            return cpp_left_2 > static_cast<std::uint32_t>(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition) * 4);
                        }()) && aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates) > 20 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_2.next(I); ) {
                                Ship = pas::list_at<aShip::TShip>(aGalaxy::Galaxy->Rangers, I);
                                if (static_cast<std::uint8_t>(reinterpret_cast<aRanger::TRanger*>(Ship)->ExcludedFromRating ^ 1) && Ship->OwnerId != aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(aShip::TShip_IsInPrison(Ship) ^ 1)) {
                                    Self->ChangeRelationToRanger(Ship, -1);
                                }
                            }
                        }
                        Self->TryDispatchPirateAttacks();
                        if (!Self->NoRandomEvents) {
                            Self->TrySpawnPirateBaseRaid();
                        }
                        Self->Government = aGalaxyStruct::pgAnarchy;
                        Self->Economy = aGalaxyStruct::peIndustrial;
                    }
                    // Native growth adds 300 even when already above the radius-based population.
                    if (Self->CalculateBasePopulation() < Self->Population) {
                        Self->Population += 300;
                    } else {
                        Self->Population += System::Trunc(Self->Population * 0.02L);
                    }
                    aPlanet::TPlanet_UpdateMarketState(Self);
                    Self->Money += System::Trunc(Self->Population * 0.001L);
                    PirateSpawnFactor = 1.0;
                    GarrisonSpawnFactor = 1.0;
                    PirateLimitFactor = 1.0;
                    GarrisonLimitFactor = 1.0;
                    if (aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) > 0) {
                        SystemRatio = aGalaxy::TGalaxy::GetCoalitionToPirateSystemRatio();
                    } else {
                        SystemRatio = 0.0;
                    }
                    switch (aGalaxy::Galaxy->PirateWinType) {
                        case 0: {
                            PirateSpawnFactor = 1.0L + 0.125L * SystemRatio;
                            GarrisonSpawnFactor = 1.0L + 0.125L * SystemRatio;
                            break;
                        }
                        case 1: {
                            PirateSpawnFactor = 1.0L - 0.125L * SystemRatio;
                            GarrisonSpawnFactor = 1.0L + 0.375L * SystemRatio;
                            PirateLimitFactor = 1.0L - 0.125L * SystemRatio;
                            GarrisonLimitFactor = 1.0L + 0.375L * SystemRatio;
                            break;
                        }
                        case 2: {
                            PirateSpawnFactor = 1.0L + 0.375L * SystemRatio;
                            GarrisonSpawnFactor = 1.0L - 0.125L * SystemRatio;
                            PirateLimitFactor = 1.0L + 0.375L * SystemRatio;
                            GarrisonLimitFactor = 1.0L - 0.125L * SystemRatio;
                            break;
                        }
                        case 3: {
                            PirateSpawnFactor = 1.0L - 0.125L * SystemRatio;
                            GarrisonSpawnFactor = 1.0L - 0.25L * SystemRatio;
                            PirateLimitFactor = 1.0L - 0.125L * SystemRatio;
                            GarrisonLimitFactor = 1.0L - 0.25L * SystemRatio;
                            break;
                        }
                        case 5: {
                            PirateSpawnFactor = 1.25;
                            GarrisonSpawnFactor = 1.125;
                            PirateLimitFactor = 1.25;
                            GarrisonLimitFactor = 1.125;
                            break;
                        }
                    }
                    switch (aGalaxy::Galaxy->DifficultyLevels[0]) {
                        case 0: {
                            PirateSpawnFactor = PirateSpawnFactor * 0.5L;
                            GarrisonSpawnFactor = GarrisonSpawnFactor * 0.5L;
                            break;
                        }
                        case 1: {
                            PirateSpawnFactor = PirateSpawnFactor * 0.85L;
                            GarrisonSpawnFactor = GarrisonSpawnFactor * 0.85L;
                            break;
                        }
                        case 2: {
                            PirateSpawnFactor = PirateSpawnFactor * 1.0L;
                            GarrisonSpawnFactor = GarrisonSpawnFactor * 1.0L;
                            break;
                        }
                        case 3: {
                            PirateSpawnFactor = PirateSpawnFactor * 1.12L;
                            GarrisonSpawnFactor = GarrisonSpawnFactor * 1.12L;
                            break;
                        }
                        default: {
                            PirateSpawnFactor = PirateSpawnFactor * (1.12L + (aGalaxy::Galaxy->DifficultyLevels[0] - 3) * 0.12L);
                            GarrisonSpawnFactor = GarrisonSpawnFactor * (1.12L + (aGalaxy::Galaxy->DifficultyLevels[0] - 3) * 0.12L);
                            break;
                        }
                    }
                    PirateLimitFactor = PirateLimitFactor * pas::real_min<pas::Extended>(2.0L, 1.0L + 0.04L * SystemRatio * SystemRatio);
                    GarrisonLimitFactor = GarrisonLimitFactor * pas::real_min<pas::Extended>(2.0L, 1.0L + 0.04L * SystemRatio * SystemRatio);
                    Budget = 2.0E+2;
                    if (Self->CurrentStar->Constellation->Id != 20) {
                        N = 0;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, std::min<std::int32_t>(10, pas::list_count(aGalaxy::Galaxy->Stars) - 1)); cpp_range_3.next(I); ) {
                            NearbyStar = Self->CurrentStar->StarDistances[I].Star;
                            if (NearbyStar->Status.ControlFaction == aGalaxyStruct::sfDominators || NearbyStar->Status.CustomFaction != u"") {
                                --N;
                            } else if (NearbyStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                                N += 2;
                            }
                        }
                        PirateSpawnFactor = static_cast<long double>(PirateSpawnFactor) * aMyFunction::RemapClamped(N, -1.0E+1, 2.0E+1, 0.5, 2.0);
                        GarrisonSpawnFactor = static_cast<long double>(GarrisonSpawnFactor) * aMyFunction::RemapClamped(N, -1.0E+1, 2.0E+1, 0.5, 2.0);
                        PirateLimitFactor = static_cast<long double>(PirateLimitFactor) * aMyFunction::RemapClamped(N, -1.0E+1, 2.0E+1, 0.8, 1.2);
                        GarrisonLimitFactor = static_cast<long double>(GarrisonLimitFactor) * aMyFunction::RemapClamped(N, -1.0E+1, 2.0E+1, 0.8, 1.2);
                        Budget = aMyFunction::RemapClamped(N, -1.0E+1, 2.0E+1, 5.0E+1, 2.0E+2);
                    }
                    if (Self->CurrentStar->Constellation->Id != 20 || aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar) {
                        PirateKills = 0;
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_4.next(I); ) {
                            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                            if (Ship->OwnerId != aGalaxyStruct::oiPirate && pas::class_cast_if<aNormalShip::TNormalShip*>(Ship) != nullptr) {
                                PirateKills += static_cast<aNormalShip::TNormalShip*>(Ship)->CurrentSystemKills.Pirate;
                            }
                        }
                        PirateSpawnFactor = PirateSpawnFactor * (1.0L - pas::real_divide(PirateKills, 15.0L));
                        GarrisonSpawnFactor = GarrisonSpawnFactor * (1.0L - pas::real_divide(PirateKills, 15.0L));
                    }
                    if (Self->CurrentStar->Status.Battle != 0) {
                        PirateSpawnFactor = PirateSpawnFactor * 0.4L;
                        GarrisonSpawnFactor = GarrisonSpawnFactor * 0.4L;
                    }
                    if (Self->CurrentStar->Id == aGalaxy::Galaxy->KellerResearchTargetStarId && aKling::KellerShip != nullptr) {
                        if (aKling::KellerShip->CurrentStar == Self->CurrentStar && static_cast<std::uint8_t>(aKling::KellerShip->InHyperspace ^ 1)) {
                            if (!Self->NoAutomaticShipSpawning) {
                                Self->TrySpawnDominator();
                            }
                            Self->AdvanceInventionProgress();
                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_5.next(I); ) {
                                Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                                if (Ship->OwnerId == aGalaxyStruct::oiDominator && Ship->CurrentPlanet == Self && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.2L) {
                                    Self->Government = static_cast<aGalaxyStruct::TPlanetGovernment>(aMyFunction::NextRandomIntRange(0, 4, Self->RandomState));
                                }
                            }
                        }
                    } else if (!Self->NoAutomaticShipSpawning) {
                        ClanShips = Self->CurrentStar->CountPirateForces(false, Strength, false, true);
                        IndependentShips = Self->CurrentStar->CountPirateForces(false, Strength, true, false);
                        PirateSpawnFactor = pas::real_divide(PirateSpawnFactor, pas::real_max<pas::Extended>(pas::real_divide(ClanShips, 9.0L), 1.0L));
                        GarrisonSpawnFactor = pas::real_divide(GarrisonSpawnFactor, pas::real_max<pas::Extended>(pas::real_divide(IndependentShips, 21.0L), 1.0L));
                        if (ClanShips < 3.0L * PirateLimitFactor && aGalaxy::Galaxy->PirateClanCount < (3.0L * PirateLimitFactor + 6.0L * GarrisonLimitFactor) * aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.02L * PirateSpawnFactor) {
                            Ship = static_cast<aShip::TShip*>(Self->BuyPirate(System::Round(Budget)));
                            if (Self->CurrentStar->Constellation->Id == 20 && aGalaxy::Galaxy->PirateWinType != 3) {
                                Boost = PirateLimitFactor;
                                while (Boost > 1.0L) {
                                    Ship->ImproveRandomEquipment(true);
                                    Ship->GainExperience(Ship->TotalExperience / 7, 0);
                                    Ship->SetMoney(Ship->Money / 7 * 8);
                                    Boost = Boost * 0.85L;
                                }
                                pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->TrainSkillsAutomatically();
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                            }
                        }
                        if (IndependentShips < 7.0L * GarrisonLimitFactor && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L * GarrisonSpawnFactor) {
                            Ship = static_cast<aShip::TShip*>(Self->BuyWarrior(System::Round(Budget)));
                            if (Self->CurrentStar->Constellation->Id == 20 && aGalaxy::Galaxy->PirateWinType != 3) {
                                Boost = GarrisonLimitFactor;
                                while (Boost > 1.0L) {
                                    Ship->ImproveRandomEquipment(true);
                                    Ship->GainExperience(Ship->TotalExperience / 7, 0);
                                    Ship->SetMoney(Ship->Money / 7 * 8);
                                    Boost = Boost * 0.85L;
                                }
                                pas::checked_cast<aNormalShip::TNormalShip*>(Ship)->TrainSkillsAutomatically();
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                                aShip::TShip_BuyEquipmentAtLocation(Ship, false);
                            }
                        }
                        if (Self->CurrentStar->Constellation->Id != 20 && Self->CurrentStar->CountPirateForces(false, Strength, true, false) > 0) {
                            if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0 && Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stTransport] < 5 && ([&] {
                                std::int32_t cpp_left_3 = aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) * 3;
                                return cpp_left_3 + aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 9;
                            }()) > aGalaxy::Galaxy->TransportCount && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.02L && Self->HomeTransportCount < 1) {
                                Self->SpawnTransport(0, 100);
                            }
                            if (aGalaxy::Galaxy->CoalitionDefeatedTurn > 0 && Self->CurrentStar->ShipTypeCounts[aGalaxyStruct::stTransport] < 5 && aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfPirates) * 5 > aGalaxy::Galaxy->TransportCount && aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.05L && Self->HomeTransportCount < 2) {
                                Self->SpawnTransport(0, 100);
                            }
                        }
                    }
                    if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.85L || Self->IsMainPiratePlanet) {
                        Self->AdvanceInventionProgress();
                    }
                    if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.5L || Self->IsMainPiratePlanet) {
                        Self->RefreshEquipmentShopInventory();
                    }
                }
            }
            if (aMyFunction::NextRandomUnitFloat(Self->RandomState) < 0.01L) {
                Self->GenerationSeed = Self->RandomState;
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>("Error in procedure TPlanet.NextDay"_a));
            } else {
                throw;
            }
        }
    }

    // Uses Self's random state for galaxy-wide attacks; disabled by pirate ending 3.
    void TPlanet::TryDispatchPirateAttacks() {
        aGalaxy::TStar* TargetStar{};
        std::int32_t Action{};
        std::int32_t TargetPirates{};
        std::int32_t EligibleShips{};
        aGalaxy::TStar* SourceStar{};
        std::int32_t TargetOpposition{};
        pas::Extended TargetPirateStrength{};
        pas::Extended TargetOppositionStrength{};
        pas::Extended SourcePirateStrength{};
        aShip::TShip* Ship{};
        std::int32_t DispatchLimit{};
        std::int32_t ShipIndex{};
        std::int32_t Dispatched{};
        std::int32_t Attempts{};
        std::int32_t NeighborIndex{};
        std::int32_t NeighborLimit{};
        pas::Extended SourceOppositionStrength{};
        std::uint8_t HasBlocker{};
        std::int32_t OtherPirates{};
        // Writes the captured action. Can replace action 1 with 2; otherwise a failed condition preserves the prior action.
        auto SelectAction = [&]() -> void {
            if (TargetStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                if (TargetStar->Status.CustomFaction != u"") {
                    Action = 0;
                    return;
                }
                if (TargetPirates > 0) {
                    pas::Extended cpp_left = static_cast<long double>(EligibleShips) * aPlanet::CalculatePirateAttackNeighborhoodFactor(SourceStar);
                    if (cpp_left > static_cast<long double>(TargetPirates) * aPlanet::CalculatePirateAttackNeighborhoodFactor(TargetStar) * 4.0L) {
                        Action = 1;
                    }
                }
                if (TargetPirates < TargetOpposition || TargetPirateStrength < TargetOppositionStrength) {
                    Action = 2;
                }
            } else {
                if (TargetPirates > 0 && (TargetPirates < TargetOpposition || TargetPirateStrength < TargetOppositionStrength || aMyFunction::NextRandomIntRange(1, 100, this->RandomState) <= 5)) {
                    Action = 3;
                }
                if (TargetPirates == 0 && (TargetOpposition < EligibleShips || TargetOppositionStrength < SourcePirateStrength || MainPiratePlanet != nullptr && SourceStar == MainPiratePlanet->CurrentStar)) {
                    Action = 4;
                }
            }
        };
        // Counts eligible ships in the source star and overwrites the captured current-ship slot while scanning.
        auto CountEligibleShips = [&]() -> std::int32_t {
            std::int32_t Count = 0;
            std::int32_t I = 0;
            while (I < pas::list_count(SourceStar->Ships)) {
                Ship = pas::list_at<aShip::TShip>(SourceStar->Ships, I);
                ++I;
                if (aPlanet::IsShipEligible(Ship)) {
                    ++Count;
                }
            }
            return Count;
        };
        // Issues absolute jumps and increments RaidPressure. Action 4 can instead schedule an idle Dominion for CurrentTurn+10. May publish an ArtAnalyzer warning.
        auto DispatchShips = [&]() -> void {
            aRuins::TRuins* Dominion{};
            pas::WideString Text{};
            if (Action == 4 && SourceStar->Dominion != nullptr && TargetStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                Dominion = reinterpret_cast<aRuins::TRuins*>(SourceStar->Dominion);
                if (Dominion->InNormalSpace() && Dominion->Order == aShip::soNone && Dominion->ScriptShip == nullptr && static_cast<std::uint8_t>(Dominion->HasScriptControl() ^ 1) && Dominion->FlyToStar == nullptr && aMyFunction::NextRandomIntRange(0, 100, this->RandomState) > 70) {
                    Dominion->FlyToStar = TargetStar;
                    Dominion->FlyDate = aGalaxy::Galaxy->CurrentTurn + 10;
                    if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && TargetStar->Status.CustomFaction == u"") {
                        Text = ([&] {
                            auto name = pas::borrow(TargetStar->Name);
                            pas::WideString localizedText = aConst::LocalizedText(u"Artefacts.ArtAnalyzer.AttackPirates"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
                        }());
                        if (Text != u"") {
                            Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                        }
                    }
                    return;
                }
            }
            std::int32_t Quarter = EligibleShips / 4;
            if (Action == 1) {
                DispatchLimit = Quarter;
            } else {
                DispatchLimit = EligibleShips - Quarter;
            }
            ShipIndex = 0;
            Dispatched = 0;
            while (ShipIndex < pas::list_count(SourceStar->Ships) && Dispatched <= DispatchLimit && DispatchLimit > 0) {
                Ship = pas::list_at<aShip::TShip>(SourceStar->Ships, ShipIndex);
                ++ShipIndex;
                if (!aPlanet::IsShipEligible(Ship)) {
                    continue;
                }
                Ship->OrderJump(TargetStar, true);
                reinterpret_cast<aPirate::TPirate*>(Ship)->RaidPressure = reinterpret_cast<aPirate::TPirate*>(Ship)->RaidPressure + 1.0L;
                ++Dispatched;
            }
            if (Action == 4 && Dispatched > 0 && TargetStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && TargetStar->Status.CustomFaction == u"") {
                    Text = ([&] {
                        auto name_2 = pas::borrow(TargetStar->Name);
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"Artefacts.ArtAnalyzer.AttackPirates"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_2.get());
                    }());
                    if (Text != u"") {
                        Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                    }
                }
            }
        };
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        std::uint8_t ControlPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates);
        std::int32_t BaseChance = System::Round(aGalaxy::Galaxy->ScaleDifficultyExponentially(aGalaxy::Galaxy->GetPirateAggressionLevel(), 5.0f, 2.0f));
        std::int32_t ControlLimit = 15 + System::Round(aGalaxy::Galaxy->GetPirateAggressionLevel() * 5 * 0.125L);
        switch (aGalaxy::Galaxy->PirateWinType) {
            case 1: {
                BaseChance *= 2;
                ControlLimit *= 2;
                break;
            }
            case 2: {
                BaseChance = System::Round(BaseChance * 0.75L);
                ControlLimit = System::Round(ControlLimit * 1.5L);
                break;
            }
            case 3: {
                BaseChance = 0;
                ControlLimit = 0;
                break;
            }
            case 5: {
                BaseChance = System::Round(BaseChance * 0.5L);
                ControlLimit = System::Round(ControlLimit * 2);
                break;
            }
        }
        std::int32_t Chance = System::Round(aMyFunction::RemapClamped(ControlPercent, 1.0, ControlLimit, BaseChance, 0.0));
        Chance = System::Round(static_cast<long double>(Chance) * aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[2], 0.0, 5.0, 1.0, 0.3));
        if (Chance == 0) {
            return;
        }
        std::int32_t MinimumFleet = 5;
        std::int32_t StarIndex = 0;
        while (StarIndex < pas::list_count(aGalaxy::Galaxy->Stars)) {
            SourceStar = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
            if (SourceStar->Constellation->Id == 20) {
                NeighborLimit = 15;
            } else {
                NeighborLimit = 10;
            }
            ++StarIndex;
            if (SourceStar->Status.ControlFaction != aGalaxyStruct::sfPirates || SourceStar->Status.CustomFaction != u"" || SourceStar->Status.Battle != 0) {
                continue;
            }
            HasBlocker = false;
            OtherPirates = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(SourceStar->Ships) - 1); cpp_range.next(Attempts); ) {
                Ship = pas::list_at<aShip::TShip>(SourceStar->Ships, Attempts);
                if (pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                    HasBlocker = true;
                    break;
                }
                if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionPassive, aGalaxyStruct::ssCoalitionPassive) && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    ++OtherPirates;
                }
            }
            if (HasBlocker) {
                continue;
            }
            SourceStar->CountPirateForces(true, SourcePirateStrength, true, true);
            EligibleShips = CountEligibleShips();
            if (EligibleShips / 4 < OtherPirates || EligibleShips < MinimumFleet) {
                continue;
            }
            if (aMyFunction::NextRandomIntRange(1, 1000, RandomState) > Chance) {
                continue;
            }
            SourceStar->CountForcesByOwnerGroups(SourceOppositionStrength, true, true, false, false);
            Action = 0;
            Attempts = NeighborLimit;
            NeighborIndex = aMyFunction::NextRandomIntRange(1, NeighborLimit, RandomState);
            while (Attempts > 0 && Action == 0) {
                TargetStar = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(SourceStar->StarDistances[NeighborIndex].Star));
                NeighborIndex = aMyFunction::IncrementWrapped(NeighborIndex, 1, NeighborLimit);
                --Attempts;
                if (TargetStar->Constellation->Id == 20 || TargetStar == SourceStar || aScript::IsStarProtectedByScript(TargetStar)) {
                    continue;
                }
                if (aGalaxy::Galaxy->CurrentTurn <= 300) {
                    if (aPlayer::GetPlayer()->CurrentStar == TargetStar) {
                        continue;
                    }
                    if (pas::sqr((1.0L - pas::real_divide(aGalaxy::Galaxy->CurrentTurn, 3.0E+2L)) * 7.0E+1L + 3.0E+1L) > aMyFunction::PointDistanceSquared(TargetStar->Position, aPlayer::GetPlayer()->CurrentStar->Position)) {
                        continue;
                    }
                }
                TargetPirates = TargetStar->CountPirateForces(false, TargetPirateStrength, true, true);
                TargetOpposition = TargetStar->CountForcesByOwnerGroups(TargetOppositionStrength, true, true, false, false);
                if (([&] {
                    pas::Extended cpp_left = aPlanet::CalculatePirateAttackNeighborhoodFactor(SourceStar);
                    return cpp_left > aPlanet::CalculatePirateAttackNeighborhoodFactor(TargetStar);
                }()) && aMyFunction::NextRandomUnitFloat(RandomState) < 0.5L) {
                    continue;
                }
                SelectAction();
                if ((Action == 4 || Action == 3 || Action == 2) && ([&] {
                    std::int32_t cpp_left_2 = aMyFunction::NextRandomIntRange(1, 60, RandomState);
                    return cpp_left_2 <= TargetStar->PlayerPresenceLevel;
                }())) {
                    continue;
                }
                if (Action != 0) {
                    DispatchShips();
                }
            }
        }
    }

    float CalculatePirateAttackNeighborhoodFactor(aGalaxy::TStar* Star) {
        std::int32_t I{};
        aGalaxy::TStar* Neighbor{};
        std::int32_t Score = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, std::min<std::int32_t>(10, pas::list_count(aGalaxy::Galaxy->Stars) - 1)); cpp_range.next(I); ) {
            Neighbor = Star->StarDistances[I].Star;
            if (Neighbor->Status.ControlFaction == aGalaxyStruct::sfDominators || Neighbor->Status.CustomFaction != u"") {
                --Score;
            } else if (Neighbor->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                Score += 2;
            }
        }
        return aMyFunction::RemapClamped(Score, -1.0E+1, 2.0E+1, 0.8, 1.2);
    }

    std::uint8_t IsShipEligible(aShip::TShip*& Ship) {
        return pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(Ship->OrderAbsolute ^ 1) && Ship->AbsoluteScriptOrder == 0 && Ship->InNormalSpace() && Ship->ScriptShip == nullptr && Ship->PartnerShip == nullptr;
    }

    // Targets a pirate base in a Coalition system; disabled by pirate endings 3 and 5.
    void TPlanet::TrySpawnPirateBaseRaid() {
        std::int32_t i{};
        std::int32_t j{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        aShip::TShip* Base{};
        std::int32_t PirateCount{};
        std::int32_t CoalitionCount{};
        std::int32_t CivilCount{};
        std::int32_t DominatorCount{};
        float Score{};
        TPlanet* SpawnPlanet{};
        aGalaxyStruct::TOwnerId OldOwner{};
        pas::WideString MessageText{};
        // Nested in TrySpawnPirateBaseRaid; unused static link is caller-popped. Scores up to ten nearby stars and maps the score to 5..20.
        auto CalculatePirateBaseRaidNeighborhoodThreshold = [&](aGalaxy::TStar* Star) -> float {
            std::int32_t i{};
            std::int32_t Score = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, std::min<std::int32_t>(10, pas::list_count(aGalaxy::Galaxy->Stars) - 1)); cpp_range.next(i); ) {
                if (Star->StarDistances[i].Star->Status.CustomFaction != u"") {
                    --Score;
                } else {
                    switch (Star->StarDistances[i].Star->Status.ControlFaction) {
                        case aGalaxyStruct::sfCoalition: Score += 2; break;
                        case aGalaxyStruct::sfPirates: Score -= 2; break;
                        case aGalaxyStruct::sfDominators: --Score; break;
                    }
                }
            }
            return aMyFunction::RemapClamped(Score, -2.0E+1, 2.0E+1, 5.0, 2.0E+1);
        };
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (pas::in_set<3, 3, 5, 5>(aGalaxy::Galaxy->PirateWinType)) {
            return;
        }
        if (pas::imod(aGalaxy::Galaxy->CurrentTurn, 55 - 5 * System::Round(aGalaxy::Galaxy->GetPirateAggressionLevel() * 0.125L)) != 0) {
            return;
        }
        pas::Extended cpp_left = static_cast<long double>(aGalaxy::Galaxy->GetPirateAggressionLevel() + 4) * aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetCoalitionToPirateSystemRatio(), 0.3, 3.0, 0.1, 1.0);
        std::int32_t Chance = System::Round(cpp_left * aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[2], 0.0, 5.0, 1.0, 0.3));
        if (aMyFunction::NextRandomIntRange(1, 100, RandomState) > Chance) {
            return;
        }
        aGalaxy::TStar* TargetStar = nullptr;
        aShip::TShip* TargetBase = nullptr;
        float BestScore = 0.0f;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(i); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, i);
            if (Star->Status.ControlFaction != aGalaxyStruct::sfCoalition || Star->Status.CustomFaction != u"" || Star->Status.Battle != 0 || Star->NoComeKling || aScript::IsStarProtectedByScript(Star)) {
                continue;
            }
            if (aGalaxy::Galaxy->CurrentTurn <= 300) {
                if (aPlayer::GetPlayer()->CurrentStar == Star) {
                    continue;
                }
                if (pas::sqr((1.0L - pas::real_divide(aGalaxy::Galaxy->CurrentTurn, 3.0E+2L)) * 7.0E+1L + 25.0L) > aMyFunction::PointDistanceSquared(Star->Position, aPlayer::GetPlayer()->CurrentStar->Position)) {
                    continue;
                }
            }
            PirateCount = 0;
            CoalitionCount = 0;
            CivilCount = 0;
            DominatorCount = 0;
            Base = nullptr;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(j); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, j);
                if (pas::is_one_of<aGalaxyStruct::ssDominator, aGalaxyStruct::ssCustom>(Ship->CurrentStanding)) {
                    ++DominatorCount;
                } else if (Ship->OwnerId == aGalaxyStruct::oiPirate) {
                    ++PirateCount;
                } else if (pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stTransport>(Ship->TypeId)) {
                    ++CivilCount;
                } else if (pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(Ship->CurrentStanding)) {
                    ++CoalitionCount;
                }
                if (Ship->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase) && Ship->InNormalSpace() && Ship->ScriptShip == nullptr) {
                    Base = Ship;
                }
            }
            if (Base == nullptr || DominatorCount > 0 || PirateCount > 0) {
                continue;
            }
            {
                pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(10, 15, RandomState);
                Score = cpp_left_2 * pas::real_divide(CalculatePirateBaseRaidNeighborhoodThreshold(Star), CivilCount + 10 + CoalitionCount * 2);
            }
            if (TargetStar == nullptr || Score > BestScore) {
                TargetStar = Star;
                BestScore = Score;
                TargetBase = Base;
            }
        }
        if (TargetStar == nullptr) {
            return;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, aMyFunction::NextRandomIntRange(6, 8, RandomState)); cpp_range_3.next(i); ) {
            SpawnPlanet = static_cast<TPlanet*>(TargetStar->SelectRandomInhabitedPlanet());
            OldOwner = SpawnPlanet->OwnerId;
            SpawnPlanet->OwnerId = aGalaxyStruct::oiPirate;
            Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(SpawnPlanet->BuyWarrior(100)));
            Ship->Position = TargetBase->Position;
            Ship->CurrentPlanet = nullptr;
            Ship->DockedTo = TargetBase;
            reinterpret_cast<aPirate::TPirate*>(Ship)->RaidPressure = 1.0f;
            reinterpret_cast<aNormalShip::TNormalShip*>(Ship)->TrainSkillsAutomatically();
            SpawnPlanet->OwnerId = OldOwner;
        }
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
            MessageText = ([&] {
                auto name = pas::borrow(TargetStar->Name);
                pas::WideString localizedText = aConst::LocalizedText(u"Artefacts.ArtAnalyzer.AttackPirates"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
            }());
            if (MessageText != u"") {
                Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, MessageText, u""_wref.get());
            }
        }
    }

    // Always initializes LastFilmPosition; optionally creates film entries for the planet and satellites.
    void TPlanet::InitializeFilmState(std::int32_t StepIndex, std::uint8_t RecordFilm) {
        TSputnik* Satellite{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t Icon{};
        std::int32_t Stage = 0;
        std::int32_t Reserved = 4;
        try {
            LastFilmPosition = EC_Struct::TruncatePointF(GetPosition());
            Stage = 1;
            Stage = 2;
            if (RecordFilm) {
                Stage = 3;
                FilmObject = Globals::PrimaryFilm->AddObject(Id, Graphic, 0, 0);
                Stage = 4;
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, FilmObject, GetPosition());
                if (CustomFaction == u"") {
                    Icon = OwnerId;
                } else {
                    Icon = aConst::GetCustomFactionPlanetIconNumber(CustomFaction);
                    if (Icon < 0) {
                        Icon = OwnerId;
                    } else {
                        Icon = Icon + 1 + 7;
                    }
                }
                Globals::PrimaryFilm->SetPlanetState(StepIndex, FilmObject, Graphic->RotationTimerInterval, Graphic->SurfaceMapStep, System::Round(OrbitalVelocity * 1.0E+3L), Graphic->RingKind, Icon);
                Stage = 5;
                Globals::PrimaryFilm->AttachObject(StepIndex, FilmObject);
                Stage = 6;
                Count = pas::list_count(Satellites);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                    Stage = 7;
                    Satellite = pas::list_at<TSputnik>(Satellites, Index);
                    Stage = 8;
                    Satellite->FilmObject = Globals::PrimaryFilm->AddObject(Satellite->Id, Satellite->Graphic, 0, 0);
                    Stage = 9;
                    Globals::PrimaryFilm->SetObjectOrbitCenter(StepIndex, Satellite->FilmObject, GetPosition());
                    Stage = 10;
                    {
                        EC_Buf::TBufEC* buildStateBuffer = Satellite->Graphic->BuildStateBuffer();
                        aEFilm::TEFilmObj* filmObject = Satellite->FilmObject;
                        aEFilm::TEFilm* primaryFilm = Globals::PrimaryFilm;
                        primaryFilm->SetObjectStateBuffer(StepIndex, filmObject, buildStateBuffer);
                    }
                    Stage = 11;
                    Globals::PrimaryFilm->AttachObject(StepIndex, Satellite->FilmObject);
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TPlanet.StepDayStart, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TPlanet::AdvanceOrbitStep(std::int32_t StepIndex, std::uint8_t RecordFilm) {
        WindowsSdk::TPoint Point{};
        TSputnik* Satellite{};
        std::int32_t Index{};
        std::int32_t Count{};
        Orbit.AngleDegrees = CurrentStar->MovementStepScale * OrbitalVelocity + Orbit.AngleDegrees;
        if (RecordFilm) {
            Point = EC_Struct::TruncatePointF(GetPosition());
            if (LastFilmPosition.X != Point.X || LastFilmPosition.Y != Point.Y) {
                Globals::PrimaryFilm->SetObjectPosition(StepIndex, FilmObject, EC_Struct::PointToPointF(Point));
                LastFilmPosition = Point;
                Count = pas::list_count(Satellites);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                    Satellite = pas::list_at<TSputnik>(Satellites, Index);
                    Globals::PrimaryFilm->SetObjectOrbitCenter(StepIndex, Satellite->FilmObject, EC_Struct::PointToPointF(Point));
                }
            }
        }
    }

    EC_Struct::TPointF TPlanet::PredictPosition(std::int32_t StepsAhead) {
        EC_Struct::TPointF Result{};
        aMyFunction::TPolarPoint Polar{};
        Polar.Radius = Orbit.Radius;
        Polar.AngleDegrees = CurrentStar->MovementStepScale * OrbitalVelocity * StepsAhead + Orbit.AngleDegrees;
        Result = aMyFunction::PolarToPoint(Polar);
        return Result;
    }

    // Queues planet dialogue to the UI thread and waits for its event; requires normal-space player state.
    std::uint8_t TPlanet_RequestDialog(TPlanet* Self) {
        std::uint8_t Result{};
        if (GR_Main::ExitScreenLoop || static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            return false;
        }
        Globals::TalkShip = nullptr;
        Globals::TalkPlanet = Self;
        Globals::TalkScripted = true;
        WindowsSdk::ResetEvent(Globals::TalkCompletedEvent);
        WindowsSdk::SetEvent(Globals::TalkRequestEvent);
        if (WindowsSdk::WaitForSingleObject(Globals::TalkCompletedEvent, WindowsSdk::INFINITE) != WindowsSdk::WAIT_OBJECT_0) {
            Result = false;
            WindowsSdk::ResetEvent(Globals::TalkRequestEvent);
            return Result;
        }
        SysUtilsImports::Sleep(10u);
        return true;
    }

    void TPlanet::UpdateOwnerFlags() {
        IsCoalitionOwned = pas::contains(aConst::PlanetOwnerMasks.Coalition, OwnerId);
    }

    void TPlanet_UpdateMarketState(TPlanet* Self) {
        std::uint8_t ItemType{};
        float TargetPrice{};
        float PriceStep{};
        float EconomyFactor{};
        std::int32_t TargetStock{};
        std::int32_t StockStep{};
        std::int32_t StoredUnits{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (pas::in_set<1, 2>(Self->ShopUpdateMode)) {
            return;
        }
        for (ItemType = static_cast<std::uint8_t>(0); ItemType <= static_cast<std::uint8_t>(7); ++ItemType) {
            if (Self->GoodsScarcityTicks[ItemType] > 0) {
                Self->ForceGoodsScarcity(false, pas::make_set<aGalaxyStruct::TItemTypeMask>({{static_cast<std::int32_t>(ItemType)}}));
            }
            if (Self->GoodsSurplusTicks[ItemType] > 0) {
                Self->ForceGoodsSurplus(false, pas::make_set<aGalaxyStruct::TItemTypeMask>({{static_cast<std::int32_t>(ItemType)}}));
            }
            StoredUnits = aPlayer::GetPlayer()->CountStoredItemUnits(Self, static_cast<aConst::TItemType>(ItemType));
            if (aPlayer::GetPlayer()->CurrentPlanet == Self && aPlayer::GetPlayer()->ConsecutiveDockedDays > 1) {
                StoredUnits += aPlayer::GetPlayer()->CargoGoods[ItemType].Count;
            }
            EconomyFactor = pas::load_unaligned<float>(pas::byte_offset(&aConst::GoodsMarket[ItemType].EconomyFactors, Self->Economy * sizeof(float)));
            if (pas::contains(aConst::PlanetOwnerMasks.PirateClan, Self->OwnerId)) {
                EconomyFactor = static_cast<long double>(EconomyFactor) * aConst::GoodsMarket[ItemType].PirateEconomyFactor;
            }
            TargetStock = System::Round(static_cast<long double>(aConst::GoodsMarket[ItemType].BaseStock) * aConst::PlanetRaceMarket[Self->RaceId].GoodsFactors[ItemType].StockFactor * aConst::PlanetGovernmentMarket[Self->Government].GoodsFactors[ItemType].StockFactor * EconomyFactor * aMyFunction::RemapClamped(Self->Radius, 6.0E+1, 1.0E+2, 0.5, 1.5));
            TargetPrice = pas::real_divide(static_cast<long double>(aConst::GoodsMarket[ItemType].AveragePrice) * aConst::PlanetRaceMarket[Self->RaceId].GoodsFactors[ItemType].PriceFactor * aConst::PlanetGovernmentMarket[Self->Government].GoodsFactors[ItemType].PriceFactor, EconomyFactor);
            if (Self->Goods[ItemType].Count + StoredUnits < TargetStock) {
                TargetPrice = pas::real_divide(TargetPrice, aMyFunction::RemapClamped(Self->Goods[ItemType].Count + StoredUnits, TargetStock * 0.1L, TargetStock, 0.8, 1.0));
            } else {
                TargetPrice = pas::real_divide(TargetPrice, aMyFunction::RemapClamped(Self->Goods[ItemType].Count + StoredUnits, TargetStock, TargetStock * 3, 1.0, 1.2));
            }
            if (static_cast<long double>(aConst::GoodsMarket[ItemType].MinPrice) < TargetPrice) {
                TargetPrice = pas::real_min<float>(TargetPrice, static_cast<float>(aConst::GoodsMarket[ItemType].MaxPrice + 1));
            } else {
                TargetPrice = pas::real_max<float>(TargetPrice, static_cast<float>(aConst::GoodsMarket[ItemType].MinPrice - 1));
            }
            if (static_cast<long double>(Self->Goods[ItemType].PriceState) - TargetPrice >= 0.0L) {
                PriceStep = static_cast<long double>(TargetPrice) * aMyFunction::NextRandomFloatRange(0.005, 0.008, Self->RandomState);
            } else {
                PriceStep = static_cast<long double>(-TargetPrice) * aMyFunction::NextRandomFloatRange(0.005, 0.008, Self->RandomState);
            }
            {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(1, 100, Self->RandomState);
                if (cpp_case >= 1 && cpp_case <= 70) {
                    Self->Goods[ItemType].PriceState = static_cast<long double>(Self->Goods[ItemType].PriceState) - PriceStep;
                } else if (cpp_case >= 71 && cpp_case <= 90) {
                } else {
                    Self->Goods[ItemType].PriceState = static_cast<long double>(Self->Goods[ItemType].PriceState) + PriceStep;
                }
            }
            if (static_cast<long double>(aConst::GoodsMarket[ItemType].MinPrice / 2) > Self->Goods[ItemType].PriceState) {
                Self->Goods[ItemType].PriceState = aConst::GoodsMarket[ItemType].MinPrice / 2;
            } else if (static_cast<long double>(aConst::GoodsMarket[ItemType].MaxPrice * 2) < Self->Goods[ItemType].PriceState) {
                Self->Goods[ItemType].PriceState = aConst::GoodsMarket[ItemType].MaxPrice * 2;
            }
            Self->Goods[ItemType].PurchasePrice = std::max<std::int64_t>(static_cast<std::int64_t>(2), System::Round(Self->Goods[ItemType].PriceState));
            Self->Goods[ItemType].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(Self->Goods[ItemType].PurchasePrice / 2 + 1), System::Round(static_cast<long double>(Self->Goods[ItemType].PriceState) * aMyFunction::RemapClamped(Self->Goods[ItemType].Count + StoredUnits, TargetStock, TargetStock * 2.2L, 0.99, 0.5) - 1.0L));
            if (Self->Goods[ItemType].Count + StoredUnits - TargetStock >= 0) {
                pas::Extended cpp_left = static_cast<long double>(TargetStock) * aMyFunction::NextRandomFloatRange(0.0025, 0.005, Self->RandomState);
                StockStep = System::Round(cpp_left + aMyFunction::NextRandomUnitFloat(Self->RandomState));
            } else {
                pas::Extended cpp_left_2 = static_cast<long double>(-TargetStock) * aMyFunction::NextRandomFloatRange(0.0025, 0.005, Self->RandomState);
                StockStep = System::Round(cpp_left_2 - aMyFunction::NextRandomUnitFloat(Self->RandomState));
            }
            {
                std::int32_t cpp_case_2 = aMyFunction::NextRandomIntRange(1, 100, Self->RandomState);
                if (cpp_case_2 >= 1 && cpp_case_2 <= 20) {
                    Self->Goods[ItemType].Count -= StockStep;
                } else if (cpp_case_2 >= 21 && cpp_case_2 <= 95) {
                } else {
                    Self->Goods[ItemType].Count += StockStep;
                }
            }
            if (Self->Goods[ItemType].Count < 0) {
                Self->Goods[ItemType].Count = 0;
            }
        }
    }

    void TPlanet::TriggerGovernmentRevolution() {
        aGalaxyStruct::TPlanetGovernment Candidate{};
        std::int32_t i{};
        std::int32_t Roll{};
        aRanger::TRanger* Ranger{};
        std::uint8_t ItemType{};
        aGalaxyStruct::TItemTypeMask GoodsMask{};
        aGalaxyStruct::TPlanetGovernment NewGovernment = Government;
        std::int32_t Attempts = 0;
        std::uint8_t NewsType = 1;
        do {
            Roll = aMyFunction::NextRandomIntRange(0, 100, RandomState);
            for (auto cpp_range = pas::for_downto<aGalaxyStruct::TPlanetGovernment>(aGalaxyStruct::pgDemocracy, aGalaxyStruct::pgAnarchy); cpp_range.next(Candidate); ) {
                if (aConst::PlanetRaceMarket[RaceId].GovernmentRollThresholds[Candidate] <= Roll) {
                    NewGovernment = Candidate;
                    switch (NewGovernment) {
                        case aGalaxyStruct::pgAnarchy: NewsType = 1; break;
                        case aGalaxyStruct::pgDictatorship: NewsType = 2; break;
                        case aGalaxyStruct::pgMonarchy: NewsType = 3; break;
                        case aGalaxyStruct::pgRepublic: NewsType = 4; break;
                        case aGalaxyStruct::pgDemocracy: NewsType = 5; break;
                    }
                    break;
                }
            }
            ++Attempts;
            if (Attempts > 100) {
                if (Government != aGalaxyStruct::pgAnarchy) {
                    NewGovernment = aGalaxyStruct::pgAnarchy;
                    NewsType = 1;
                } else {
                    NewGovernment = aGalaxyStruct::pgDemocracy;
                    NewsType = 5;
                }
            }
            // The native news-duplication test has an empty body.
            static_cast<void>(CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CountPlanetNewsByType(NewsType) > 0 && Attempts < 200);
        } while (!(Government != NewGovernment));
        Government = NewGovernment;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_2.next(i); ) {
            Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, i);
            if (!Ranger->ExcludedFromRating) {
                ChangeRelationToRanger(Ranger, aConst::PlanetGovernmentMarket[Government].RevolutionRelationDelta[Ranger->GetDominantCareer()]);
            }
        }
        if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            pas::WideString formatText2 = ([&] {
                auto name = pas::borrow(CurrentStar->Name);
                auto name_2 = pas::borrow(Name);
                pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(static_cast<pas::WideString>(pas::concat_ansi({"GalaxyNews.Planet.Revolution.", SysUtils::IntToStr(Government)})), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Star>"_w, name.get(), u"<Planet>"_w, name_2.get());
            }());
            aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
            galaxy->AddPlanetNews(1, std::move(formatText2));
        }
        // The goods event uses Candidate even when the attempt limit changes NewGovernment.
        switch (Candidate) {
            case aGalaxyStruct::pgAnarchy: {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}, {1}, {2}, {6}}));
                break;
            }
            case aGalaxyStruct::pgDictatorship: {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}, {1}, {6}}));
                break;
            }
            case aGalaxyStruct::pgMonarchy: {
                GoodsMask = pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}});
                for (ItemType = static_cast<std::uint8_t>(1); ItemType <= static_cast<std::uint8_t>(7); ++ItemType) {
                    if (static_cast<long double>(aConst::GoodsMarket[ItemType].AveragePrice) > Goods[ItemType].PriceState) {
                        pas::include_at(&GoodsMask, ItemType);
                    }
                }
                ForceGoodsSurplus(true, GoodsMask);
                break;
            }
            case aGalaxyStruct::pgRepublic: {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{4}}));
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}, {6}}));
                break;
            }
            case aGalaxyStruct::pgDemocracy: {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{3}, {4}}));
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}, {6}}));
                break;
            }
        }
    }

    // May trigger a revolution, goods scarcity or surplus and publish planet news; honors NoRandomEvents.
    void TPlanet::TryTriggerEconomicEvent() {
        if (IsMainPiratePlanet) {
            return;
        }
        if (NoRandomEvents) {
            return;
        }
        if (CurrentStar->IsConstellationVisible() && pas::list_count(aGalaxy::Galaxy->PlanetNews) >= aConst::MaxPlanetNews) {
            return;
        }
        if (CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] > 0) {
            return;
        }
        if ((aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(GenerationSeed)) % 30 != 0) {
            return;
        }
        if (CurrentStar->DaysSincePlayerVisit < 30) {
            return;
        }
        {
            pas::Extended cpp_left = aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1017, 0.0, 1.0);
            if (cpp_left < aConst::PlanetRaceMarket[RaceId].RevolutionChance) {
                TriggerGovernmentRevolution();
            } else if (([&] {
                std::int32_t cpp_left_2 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1117);
                return cpp_left_2 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(6) == 0) && pas::is_one_of<aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy)) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}}));
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{4}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2 = ([&] {
                        auto name = pas::borrow(CurrentStar->Name);
                        auto name_2 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.MineralDeposit"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Star>"_w, name.get(), u"<Planet>"_w, name_2.get());
                    }());
                    aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                    galaxy->AddPlanetNews(6, std::move(formatText2));
                }
            } else if (([&] {
                std::int32_t cpp_left_3 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1127);
                return cpp_left_3 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(7) == 0) && pas::in_range(Economy, static_cast<std::int32_t>(aGalaxyStruct::peIndustrial), static_cast<std::int32_t>(aGalaxyStruct::peIndustrial))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{4}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_2 = ([&] {
                        auto name_3 = pas::borrow(CurrentStar->Name);
                        auto name_4 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedMineral"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_3.get(), u"<Planet>"_w, name_4.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                    galaxy_2->AddPlanetNews(7, std::move(formatText2_2));
                }
            } else if (([&] {
                std::int32_t cpp_left_4 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1217);
                return cpp_left_4 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(8) == 0) && pas::is_one_of<aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy) && pas::is_one_of<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiHuman, aGalaxyStruct::oiFeyan>(aConst::RaceToOwner(RaceId))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}}));
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{6}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_3 = ([&] {
                        auto name_5 = pas::borrow(CurrentStar->Name);
                        auto name_6 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyArms"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_3), u"<color=255,240,100>"_w, u"<Star>"_w, name_5.get(), u"<Planet>"_w, name_6.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
                    galaxy_3->AddPlanetNews(8, std::move(formatText2_3));
                }
            } else if (([&] {
                std::int32_t cpp_left_5 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1227);
                return cpp_left_5 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(9) == 0) && pas::in_range(Economy, static_cast<std::int32_t>(aGalaxyStruct::peMixed), static_cast<std::int32_t>(aGalaxyStruct::peMixed)) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiHuman))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{6}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_4 = ([&] {
                        auto name_7 = pas::borrow(CurrentStar->Name);
                        auto name_8 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedArms"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_4), u"<color=255,240,100>"_w, u"<Star>"_w, name_7.get(), u"<Planet>"_w, name_8.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                    galaxy_4->AddPlanetNews(9, std::move(formatText2_4));
                }
            } else if (([&] {
                std::int32_t cpp_left_6 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1237);
                return cpp_left_6 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(9) == 0) && pas::in_range(Economy, static_cast<std::int32_t>(aGalaxyStruct::peMixed), static_cast<std::int32_t>(aGalaxyStruct::peMixed)) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiFeyan)) && pas::in_range(Government, static_cast<std::int32_t>(aGalaxyStruct::pgDemocracy), static_cast<std::int32_t>(aGalaxyStruct::pgDemocracy))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{6}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_5 = ([&] {
                        auto name_9 = pas::borrow(CurrentStar->Name);
                        auto name_10 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedArmsForRevolution"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_5), u"<color=255,240,100>"_w, u"<Star>"_w, name_9.get(), u"<Planet>"_w, name_10.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_5 = aGalaxy::Galaxy;
                    galaxy_5->AddPlanetNews(9, std::move(formatText2_5));
                }
            } else if (([&] {
                std::int32_t cpp_left_7 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1317);
                return cpp_left_7 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(10) == 0) && pas::is_one_of<aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiHuman), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}, {6}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_6 = ([&] {
                        auto name_11 = pas::borrow(CurrentStar->Name);
                        auto name_12 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_6 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyTechnics"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_6), u"<color=255,240,100>"_w, u"<Star>"_w, name_11.get(), u"<Planet>"_w, name_12.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_6 = aGalaxy::Galaxy;
                    galaxy_6->AddPlanetNews(10, std::move(formatText2_6));
                }
            } else if (([&] {
                std::int32_t cpp_left_8 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 71417);
                return cpp_left_8 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(11) == 0) && pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peMixed>(Economy)) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_7 = ([&] {
                        auto name_13 = pas::borrow(CurrentStar->Name);
                        auto name_14 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_7 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyFood"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_7), u"<color=255,240,100>"_w, u"<Star>"_w, name_13.get(), u"<Planet>"_w, name_14.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_7 = aGalaxy::Galaxy;
                    galaxy_7->AddPlanetNews(11, std::move(formatText2_7));
                }
            } else if (([&] {
                std::int32_t cpp_left_9 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 31427);
                return cpp_left_9 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(11) == 0) && pas::in_range(Economy, static_cast<std::int32_t>(aGalaxyStruct::peAgricultural), static_cast<std::int32_t>(aGalaxyStruct::peAgricultural))) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}}));
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{2}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_8 = ([&] {
                        auto name_15 = pas::borrow(CurrentStar->Name);
                        auto name_16 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_8 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyFoodNeedTechnics"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_8), u"<color=255,240,100>"_w, u"<Star>"_w, name_15.get(), u"<Planet>"_w, name_16.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_8 = aGalaxy::Galaxy;
                    galaxy_8->AddPlanetNews(11, std::move(formatText2_8));
                }
            } else if (([&] {
                std::int32_t cpp_left_10 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 21437);
                return cpp_left_10 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(12) == 0) && pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiFeyan))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0}, {1}, {7}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_9 = ([&] {
                        auto name_17 = pas::borrow(CurrentStar->Name);
                        auto name_18 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_9 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedFood"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_9), u"<color=255,240,100>"_w, u"<Star>"_w, name_17.get(), u"<Planet>"_w, name_18.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_9 = aGalaxy::Galaxy;
                    galaxy_9->AddPlanetNews(12, std::move(formatText2_9));
                }
            } else if (([&] {
                std::int32_t cpp_left_11 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1517);
                return cpp_left_11 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(13) == 0) && pas::in_range(Economy, static_cast<std::int32_t>(aGalaxyStruct::peMixed), static_cast<std::int32_t>(aGalaxyStruct::peMixed)) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiHuman), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{1}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_10 = ([&] {
                        auto name_19 = pas::borrow(CurrentStar->Name);
                        auto name_20 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_10 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyMedicine"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_10), u"<color=255,240,100>"_w, u"<Star>"_w, name_19.get(), u"<Planet>"_w, name_20.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_10 = aGalaxy::Galaxy;
                    galaxy_10->AddPlanetNews(13, std::move(formatText2_10));
                }
            } else if (([&] {
                std::int32_t cpp_left_12 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1617);
                return cpp_left_12 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(14) == 0) && pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiPeleng), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{3}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_11 = ([&] {
                        auto name_21 = pas::borrow(CurrentStar->Name);
                        auto name_22 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_11 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyLuxury"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_11), u"<color=255,240,100>"_w, u"<Star>"_w, name_21.get(), u"<Planet>"_w, name_22.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_11 = aGalaxy::Galaxy;
                    galaxy_11->AddPlanetNews(14, std::move(formatText2_11));
                }
            } else if (([&] {
                std::int32_t cpp_left_13 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1717);
                return cpp_left_13 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(15) == 0) && pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peMixed>(Economy) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiHuman), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{3}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_12 = ([&] {
                        auto name_23 = pas::borrow(CurrentStar->Name);
                        auto name_24 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_12 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedLuxury"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_12), u"<color=255,240,100>"_w, u"<Star>"_w, name_23.get(), u"<Planet>"_w, name_24.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_12 = aGalaxy::Galaxy;
                    galaxy_12->AddPlanetNews(15, std::move(formatText2_12));
                }
            } else if (([&] {
                std::int32_t cpp_left_14 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1817);
                return cpp_left_14 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(16) == 0) && pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peMixed>(Economy) && pas::in_range(aConst::RaceToOwner(RaceId), static_cast<std::int32_t>(aGalaxyStruct::oiPeleng), static_cast<std::int32_t>(aGalaxyStruct::oiHuman))) {
                ForceGoodsSurplus(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{5}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_13 = ([&] {
                        auto name_25 = pas::borrow(CurrentStar->Name);
                        auto name_26 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_13 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.ManyAlcohol"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_13), u"<color=255,240,100>"_w, u"<Star>"_w, name_25.get(), u"<Planet>"_w, name_26.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_13 = aGalaxy::Galaxy;
                    galaxy_13->AddPlanetNews(16, std::move(formatText2_13));
                }
            } else if (([&] {
                std::int32_t cpp_left_15 = aMyFunction::SeededRandomIntRange(0, 100, aGalaxy::Galaxy->CurrentTurn * static_cast<std::int32_t>(GenerationSeed) * 1917);
                return cpp_left_15 < EconomicEventChance;
            }()) && (static_cast<std::uint8_t>(CurrentStar->IsConstellationVisible() ^ 1) || aGalaxy::Galaxy->CountPlanetNewsByType(17) == 0) && pas::is_one_of<aGalaxyStruct::peMixed, aGalaxyStruct::peIndustrial>(Economy) && pas::is_one_of<aGalaxyStruct::oiHuman, aGalaxyStruct::oiGaal>(aConst::RaceToOwner(RaceId))) {
                ForceGoodsScarcity(true, pas::constant_set<aGalaxyStruct::TItemTypeMask>({{5}}));
                if (CurrentStar->IsConstellationVisible() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::WideString formatText2_14 = ([&] {
                        auto name_27 = pas::borrow(CurrentStar->Name);
                        auto name_28 = pas::borrow(Name);
                        pas::WideString pickLocalizedTextVariant_14 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Planet.NeedAlcohol"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * static_cast<std::int32_t>(GenerationSeed));
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_14), u"<color=255,240,100>"_w, u"<Star>"_w, name_27.get(), u"<Planet>"_w, name_28.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_14 = aGalaxy::Galaxy;
                    galaxy_14->AddPlanetNews(17, std::move(formatText2_14));
                }
            }
        }
    }

    // Native no-op. TStar.NextDay calls this after detecting a collision; the caller handles impact effects, debris and asteroid respawn.
    void TPlanet::HandleAsteroidImpact(void* Asteroid) {
    }

    // Clears Choices, appends matching nonempty planet-binding titles and stores the owning TScript as each entry's data.
    void TPlanet::CollectScriptDialogChoices(EC_Str::TStringsEC* Choices) {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t k{};
        aScript::TScript* Script{};
        aScript::TScriptStar* Star{};
        Choices->Clear();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(i); ) {
            Script = pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, i);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Script->Stars) - 1); cpp_range_2.next(j); ) {
                Star = pas::list_at<aScript::TScriptStar>(Script->Stars, j);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Star->Planets.length() - 1); cpp_range_3.next(k); ) {
                    if (Star->Planets[k].Planet == this && Star->Planets[k].DialogChoiceText != u"") {
                        Choices->Add(Star->Planets[k].DialogChoiceText);
                        Choices->SetDataAt(Choices->GetCount() - 1, Script);
                    }
                }
            }
        }
    }

    // Returns -1 when unavailable; otherwise combines a surface family in bits 24..31 with eligible animation bits.
    std::int32_t TPlanet::GetSurfaceAnimationMask() {
        std::int32_t Index{};
        std::int32_t Family{};
        std::uint8_t GoodsIndex{};
        std::uint8_t Good{};
        aGalaxyStruct::TOwnerMask Owners{};
        std::uint8_t Eligible{};
        std::int32_t Result = -1;
        if (!pas::contains(aConst::PlanetOwnerMasks.Coalition, OwnerId)) {
            return Result;
        }
        if (Graphic->RingKind == 1) {
            Family = 0;
        } else if (Graphic->RingKind == 4) {
            Family = 1;
        } else if (Graphic->RingKind == 5) {
            Family = 2;
        } else {
            return Result;
        }
        std::uint8_t SelectedGood = 42;
        std::int32_t ConditionIndex = 0;
        for (GoodsIndex = static_cast<std::uint8_t>(0); GoodsIndex <= static_cast<std::uint8_t>(7); ++GoodsIndex) {
            if (aConst::GoodsLegalOnPlanet[GoodsIndex][RaceId][Government]) {
                if (Goods[GoodsIndex].Count >= aConst::GoodsMarket[GoodsIndex].BaseStock / 2) {
                    Index = Goods[GoodsIndex].PurchasePrice - (aConst::GoodsMarket[GoodsIndex].MinPrice + aConst::GoodsMarket[GoodsIndex].AveragePrice) / 2;
                    if (Index < 0 && Index < ConditionIndex) {
                        ConditionIndex = Index;
                        SelectedGood = GoodsIndex;
                    }
                }
            }
        }
        Globals::PPlanetAdvertDefinition Definition = &Globals::PlanetAdvertDefinitions[Family];
        Result = 0;
        {
            const std::int32_t cpp_last = Definition->Lists.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    Eligible = true;
                    {
                        const std::int32_t cpp_last_2 = Definition->Lists[Index].Indices.length() - 1;
                        if (0 <= cpp_last_2) {
                            for (ConditionIndex = 0; ConditionIndex <= cpp_last_2; ++ConditionIndex) {
                                do {
                                    switch (Definition->Adverts[Definition->Lists[Index].Indices[ConditionIndex]].War) {
                                        case -1: {
                                            if (CurrentStar->Status.Battle != 0) {
                                                Eligible = false;
                                                goto cpp_loop_exit;
                                            }
                                            break;
                                        }
                                        case 1: {
                                            if (CurrentStar->Status.Battle == 0) {
                                                Eligible = false;
                                                goto cpp_loop_exit;
                                            }
                                            break;
                                        }
                                    }
                                    Good = Definition->Adverts[Definition->Lists[Index].Indices[ConditionIndex]].Goods;
                                    if (pas::in_range(Good, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics)) && Good != SelectedGood) {
                                        Eligible = false;
                                    } else {
                                        Owners = Definition->Adverts[Definition->Lists[Index].Indices[ConditionIndex]].Owner;
                                        if (Owners != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Owners, OwnerId) ^ 1)) {
                                            Eligible = false;
                                        } else {
                                            Result |= pas::shl(1, Index);
                                        }
                                    }
                                } while (!true);
                                cpp_loop_exit:;
                                // Native sets the bit as each condition succeeds; a later failure does
                                // not clear an already set bit for the same list.
                                if (!Eligible) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (Result <= 0) {
            return -1;
        }
        return Result | pas::shl(Family, 24);
    }

    // Seed-based StyleFace selection, excluding the player's portrait.
    pas::WideString TPlanet::GetGovernmentPortraitGraph() {
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::Array<std::int32_t, 0, 50> Faces{};
        std::int32_t FaceCount = 0;
        Block = GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"StyleFace", aConst::OwnerInfo[OwnerId].InternalName}));
        std::int32_t Count = Block->GetParamCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            if (EC_Str::FindTextOffsetW(Block->GetParamValue(Index), u"L"_wref.get(), 0) >= 0) {
                Faces[FaceCount] = EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(Index)));
                if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->PortraitFaceId != Faces[FaceCount]) {
                    ++FaceCount;
                }
            }
        }
        // Native selection deliberately excludes the last candidate and assumes at least two.
        Index = Faces[pas::imod(static_cast<std::int32_t>(GenerationSeed), FaceCount - 1)];
        return pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[OwnerId].InternalName, pas::wide_int_to_str(Index)});
    }

    // Main Pirate Planet returns its name without the localized planet prefix.
    pas::WideString TPlanet::GetFullName(pas::WideString Separator) {
        if (IsMainPiratePlanet) {
            return Name;
        }
        return pas::concat_wide({aConst::LocalizedText(u"Planet.Name"_wref.get()), Separator, Name});
    }

    EC_Struct::TPointF TPlanet::GetPosition() {
        EC_Struct::TPointF Result{};
        Result = aMyFunction::PolarToPoint(Orbit);
        return Result;
    }

    // ForMap suppresses the artifact treasure hint and can append a Pirate Clan warning.
    pas::WideString TPlanet::GetInfoText(std::uint8_t ForMap) {
        pas::WideString Text{};
        if (IsMainPiratePlanet) {
            if (OwnerId == aGalaxyStruct::oiPirate) {
                Text = aConst::LocalizedText(u"Planet.MainPiratePlanet.Info.TextAboutPlanet"_wref.get());
            } else {
                Text = aConst::LocalizedText(u"Planet.MainPiratePlanet.Info.TextAboutPlanetAlt"_wref.get());
            }
        } else if (CustomFaction != u"" && OwnerId != aGalaxyStruct::oiUninhabited) {
            Text = aConst::LocalizedText(pas::concat_wide({u"Planet.", CustomFaction, u".Info.TextAboutPlanet"}));
        } else if (CurrentStar->Status.CustomFaction != u"" && OwnerId != aGalaxyStruct::oiUninhabited) {
            Text = aConst::LocalizedText(pas::concat_wide({u"Planet.", CurrentStar->Status.CustomFaction, u".Info.TextAboutPlanet"}));
        } else {
            aGalaxyStruct::TOwnerId cpp_case = OwnerId;
            if (cpp_case >= aGalaxyStruct::oiMaloc && cpp_case <= aGalaxyStruct::oiGaal || cpp_case == aGalaxyStruct::oiPirate) {
                Text = aConst::LocalizedText(u"Planet.Civil.Info.TextAboutPlanet"_wref.get());
            } else if (cpp_case == aGalaxyStruct::oiDominator) {
                Text = aConst::LocalizedText(u"Planet.Kling.Info.TextAboutPlanet"_wref.get());
            } else if (cpp_case == aGalaxyStruct::oiUninhabited) {
                Text = aConst::LocalizedText(u"Planet.NotCivil.Info.TextAboutPlanet"_wref.get());
            }
        }
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && OwnerId == aGalaxyStruct::oiUninhabited && static_cast<std::uint8_t>(ForMap ^ 1)) {
                Text = pas::concat_wide({Text, u"\r\n", BuildNonCivilTreasureHintText()});
            }
        }
        if (ForMap && OwnerId == aGalaxyStruct::oiPirate && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            Text = pas::concat_wide({Text, u"\r\n", u"<color=255,0,0>", aConst::LocalizedText(u"Planet.Civil.Info.TextPlanetControlledByPirates"_wref.get()), u"</color>"});
        }
        if (WaterTiles - WaterExplored > 0) {
            aMyFunction::ReplaceTextToken(Text, u"<Water>"_w, pas::wide_int_to_str(WaterTiles - WaterExplored), u"<color=255,240,100>"_w);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Water>"_w, u"-"_w, pas::WideString());
        }
        if (LandTiles - LandExplored > 0) {
            aMyFunction::ReplaceTextToken(Text, u"<Land>"_w, pas::wide_int_to_str(LandTiles - LandExplored), u"<color=255,240,100>"_w);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Land>"_w, u"-"_w, pas::WideString());
        }
        if (HillTiles - HillExplored > 0) {
            aMyFunction::ReplaceTextToken(Text, u"<Hill>"_w, pas::wide_int_to_str(HillTiles - HillExplored), u"<color=255,240,100>"_w);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Hill>"_w, u"-"_w, pas::WideString());
        }
        aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, CurrentStar->Name, u"<color=255,240,100>"_w);
        if (IsMainPiratePlanet) {
            aMyFunction::ReplaceTextToken(Text, u"<Race>"_w, aConst::OwnerInfo[OwnerId].DisplayName, u"<color=255,240,100>"_w);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Race>"_w, GetNativeRaceName(), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(Text, u"<Population>"_w, pas::wide_int64_to_str(System::Round(pas::real_divide(Population, 1.0E+3L))), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Economy>"_w, aConst::PlanetEconomyInfo[Economy].DisplayName, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Goverment>"_w, GetGovernmentName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Relation>"_w, GetRelationLevelTextToShip(aPlayer::GetPlayer()), u"<color=255,240,100>"_w);
        return Text;
    }

    pas::WideString TPlanet::GetGovernmentName() {
        return aConst::PlanetGovernmentMarket[Government].DisplayName;
    }

    // Localized DisplayName for RaceToOwner(RaceId).
    pas::WideString TPlanet::GetNativeRaceName() {
        return aConst::OwnerInfo[aConst::RaceToOwner(RaceId)].DisplayName;
    }

    // Faction/series/internal owner identifier used for resource selection.
    pas::WideString TPlanet::GetFactionResourceName() {
        if (CustomFaction != u"") {
            return CustomFaction;
        } else if (CurrentStar->Status.CustomFaction != u"") {
            return CurrentStar->Status.CustomFaction;
        } else if (CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
            return aConst::DominatorSeriesNames[CurrentStar->Status.DominatorSeries];
        } else if (IsMainPiratePlanet) {
            return aConst::OwnerInfo[OwnerId].InternalName;
        } else if (OwnerId == aGalaxyStruct::oiPirate) {
            return pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::RaceToSys(RaceId)});
        } else {
            return aConst::OwnerInfo[OwnerId].InternalName;
        }
    }

    // Maps Radius 60..100 to population 100000..1000000 with clamping and rounding.
    std::int32_t TPlanet::CalculateBasePopulation() {
        return System::Round(aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 1.0E+5, 1.0E+6));
    }

    // For OwnerId=6 counts all uninhabited planets; otherwise counts non-uninhabited planets with the same RaceId. Includes Self.
    std::int32_t TPlanet::CountPlanetsOfSameRace() {
        std::int32_t i{};
        TPlanet* Planet{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range.next(i); ) {
            Planet = pas::list_at<TPlanet>(aGalaxy::Galaxy->Planets, i);
            if (OwnerId == aGalaxyStruct::oiUninhabited) {
                if (OwnerId == Planet->OwnerId) {
                    ++Count;
                }
            } else if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
                if (RaceId == Planet->RaceId) {
                    ++Count;
                }
            }
        }
        return Count;
    }

    // First adjacent invisible constellation, excluding ID 20; borrowed result or nil.
    aGalaxy::TConstellation* TPlanet::FindUnchartedNeighborConstellation() {
        std::int32_t i{};
        aGalaxy::TConstellation* Constellation{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range.next(i); ) {
            Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, i);
            if (Constellation->SharesOutlineSegment(CurrentStar->Constellation)) {
                if (!Constellation->Visible) {
                    if (Constellation->Id != 20) {
                        return Constellation;
                    }
                }
            }
        }
        return nullptr;
    }

    // Searches stars in CurrentStar's distance order, then each star's planet list; no planet-distance tie break.
    TPlanet* TPlanet::FindNearestPlanetByOwnerMask(aGalaxyStruct::TOwnerMask OwnerMask) {
        std::int32_t i{};
        std::int32_t j{};
        aGalaxy::TStar* Star{};
        TPlanet* Planet{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(i); ) {
            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[i].Star));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(j); ) {
                Planet = pas::list_at<TPlanet>(Star->Planets, j);
                if (pas::contains(OwnerMask, Planet->OwnerId)) {
                    return Planet;
                }
            }
        }
        return nullptr;
    }

    // Sorts by SurfaceTileIndex and moves overlapping markers to free cells of the 14-by-7 display grid.
    void TPlanet::NormalizeSurfaceLootEntries() {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Direction{};
        std::int32_t X{};
        std::int32_t Y{};
        PPlanetSurfaceLootEntry First{};
        PPlanetSurfaceLootEntry Second{};
        std::int32_t Tail{};
        std::int32_t Head{};
        pas::Array<pas::Array<std::uint8_t, 0, 6>, 0, 13> Occupied{};
        pas::Array<pas::Array<std::uint8_t, 0, 6>, 0, 13> Visited{};
        pas::Array<WindowsSdk::TPoint, 0, 97> Queue{};
        if (SurfaceLootEntries == nullptr) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 2); cpp_range.next(i); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(i + 1, pas::list_count(SurfaceLootEntries) - 1); cpp_range_2.next(j); ) {
                First = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
                Second = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, j);
                if (Second->SurfaceTileIndex < First->SurfaceTileIndex) {
                    pas::list_put(SurfaceLootEntries, i, static_cast<void*>(Second));
                    pas::list_put(SurfaceLootEntries, j, static_cast<void*>(First));
                }
            }
        }
        // These native inclusive bounds overrun the 14-by-7 arrays; retained exactly.
        for (Y = 0; Y <= 7; ++Y) {
            for (X = 0; X <= 14; ++X) {
                Occupied[X][Y] = false;
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range_3.next(i); ) {
            First = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
            Occupied[First->GridX][First->GridY] = true;
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 2); cpp_range_4.next(i); ) {
            First = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(i + 1, pas::list_count(SurfaceLootEntries) - 1); cpp_range_5.next(j); ) {
                Second = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, j);
                if (First->GridX == Second->GridX && First->GridY == Second->GridY) {
                    for (Y = 0; Y <= 7; ++Y) {
                        for (X = 0; X <= 14; ++X) {
                            Visited[X][Y] = false;
                        }
                    }
                    Queue[0].X = Second->GridX;
                    Queue[0].Y = Second->GridY;
                    Visited[Queue[0].X][Queue[0].Y] = true;
                    Head = 0;
                    Tail = 1;
                    while (Head < Tail) {
                        for (Direction = 0; Direction <= 3; ++Direction) {
                            X = Queue[Head].X;
                            Y = Queue[Head].Y;
                            switch (Direction) {
                                case 0: {
                                    ++X;
                                    if (X >= 14) {
                                        continue;
                                    }
                                    break;
                                }
                                case 1: {
                                    --X;
                                    if (X < 0) {
                                        continue;
                                    }
                                    break;
                                }
                                case 2: {
                                    ++Y;
                                    if (Y >= 7) {
                                        continue;
                                    }
                                    break;
                                }
                                case 3: {
                                    --Y;
                                    if (Y < 0) {
                                        continue;
                                    }
                                    break;
                                }
                            }
                            if (Visited[X][Y]) {
                                continue;
                            }
                            if (!Occupied[X][Y]) {
                                Occupied[X][Y] = true;
                                Second->GridX = X;
                                Second->GridY = Y;
                                Head = Tail;
                                break;
                            } else {
                                if (Tail >= 98) {
                                    GR_Main::RaiseWideMessage(u"Gone item coords"_wref.get());
                                }
                                Visited[X][Y] = true;
                                Queue[Tail].X = X;
                                Queue[Tail].Y = Y;
                                ++Tail;
                            }
                        }
                        ++Head;
                    }
                }
            }
        }
    }

    std::int32_t TPlanet::GetTotalSurfaceTileCount() {
        return WaterTiles + LandTiles + HillTiles;
    }

    // Returns zero unless OwnerId=6.
    std::int32_t TPlanet::GetUnexploredSurfaceTileCount() {
        if (OwnerId == aGalaxyStruct::oiUninhabited) {
            return GetTotalSurfaceTileCount() - (WaterExplored + LandExplored + HillExplored);
        }
        return 0;
    }

    // Takes item ownership, allocates a 12-byte entry and always returns true on completion. Requires positive surface area; resets exploration if fully explored.
    std::uint8_t TPlanet::AddSurfaceLootEntry(aItem::TItem* Item) {
        PPlanetSurfaceLootEntry Entry{};
        if (GetUnexploredSurfaceTileCount() == 0) {
            WaterExplored = 0;
            LandExplored = 0;
            HillExplored = 0;
        }
        std::int32_t Total = GetTotalSurfaceTileCount();
        std::int32_t Terrain = 0;
        while (true) {
            Terrain = System::Round(aMyFunction::NextRandomIntRange(1, Total, RandomState));
            if (Terrain <= WaterTiles && WaterExplored < WaterTiles) {
                Terrain = 0;
                break;
            } else if (Terrain <= WaterTiles + LandTiles && LandExplored < LandTiles) {
                Terrain = 1;
                break;
            } else if (Terrain <= WaterTiles + LandTiles + HillTiles && HillExplored < HillTiles) {
                Terrain = 2;
                break;
            }
        }
        if (SurfaceLootEntries == nullptr) {
            SurfaceLootEntries = pas::make_object<pas::List>();
        }
        pas::get_mem_at(&Entry, static_cast<std::int32_t>(sizeof(TPlanetSurfaceLootEntry)));
        pas::list_add(SurfaceLootEntries, static_cast<void*>(Entry));
        Entry->GridX = aMyFunction::NextRandomIntRange(0, 13, RandomState);
        Entry->GridY = aMyFunction::NextRandomIntRange(0, 6, RandomState);
        Entry->TerrainKind = static_cast<TPlanetTerrainKind>(Terrain);
        switch (Terrain) {
            case 0: {
                Entry->SurfaceTileIndex = aMyFunction::SeededRandomIntRange(WaterExplored + 1, WaterTiles, RandomState);
                break;
            }
            case 1: {
                Entry->SurfaceTileIndex = aMyFunction::SeededRandomIntRange(LandExplored + 1, LandTiles, RandomState);
                break;
            }
            case 2: {
                Entry->SurfaceTileIndex = aMyFunction::SeededRandomIntRange(HillExplored + 1, HillTiles, RandomState);
                break;
            }
        }
        Entry->Unavailable = false;
        Entry->Item = Item;
        return true;
    }

    // Requires OwnerId=6, at least 720 days without a player visit and no deployed player probe here. Clears unavailable loot flags and resets exploration if any flag changed.
    std::uint8_t TPlanet::TryResetSurfaceLootAfterLongAbsence() {
        std::int32_t i{};
        PPlanetSurfaceLootEntry Entry{};
        std::uint8_t Result = false;
        if (CurrentStar->DaysSincePlayerVisit < 720) {
            return Result;
        }
        if (OwnerId != aGalaxyStruct::oiUninhabited) {
            return Result;
        }
        if (GetUnexploredSurfaceTileCount() > GetTotalSurfaceTileCount() * 0.4L) {
            return Result;
        }
        if (aPlayer::GetPlayer()->HasSatelliteOnPlanet(this)) {
            return Result;
        }
        if (SurfaceLootEntries == nullptr) {
            return Result;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range.next(i); ) {
            Entry = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, i);
            if (Entry->Unavailable && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanAccessSurfaceLootItem(Entry->Item))) {
                Entry->Unavailable = false;
                Result = true;
            }
        }
        if (Result) {
            WaterExplored = 0;
            LandExplored = 0;
            HillExplored = 0;
        }
        return Result;
    }

    // Increments the current track, clears its progress and selects the next track after every increment.
    void TPlanet::BoostInventionLevels(std::int32_t Count) {
        std::int32_t Index{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(Index); ) {
            ++InventionLevels[CurrentInvention];
            CurrentInventionPoints = 0.0f;
            SelectCurrentInvention();
        }
    }

    // Chooses among tracks permitted by ResearchLevelPercent and main technology slot 7; raises if no choice is found.
    void TPlanet::SelectCurrentInvention() {
        std::uint8_t Track{};
        std::int32_t i{};
        std::int32_t Index{};
        std::uint8_t Found = false;
        double Chance = 0.03;
        do {
            Index = aMyFunction::NextRandomIntRange(0, 19, RandomState);
            for (i = 0; i <= 19; ++i) {
                aMyFunction::IncrementWrapped(Index, 0, 19);
                Track = Index;
                if (ResearchLevelPercent > System::Round(InventionLevels[Track] * 12.5L) && aConst::PlanetInventionInfo[Track].RequiredMainTechLevel <= InventionLevels[7] && InventionLevels[Track] <= InventionLevels[7] && aMyFunction::NextRandomUnitFloat(RandomState) <= Chance) {
                    CurrentInvention = Track;
                    Found = true;
                    break;
                }
            }
            Chance = Chance + 0.03L;
            if (Chance > 1.2L) {
                pas::raise(pas::make_exception<pas::Exception>("Error in TPlanet.SetCurInvention"_a));
            }
        } while (!Found);
    }

    // Uses the difficulty multiplier; completion requires progress strictly above 100. Levels cap at 8 and excess progress is discarded.
    void TPlanet::AdvanceInventionProgress() {
        std::uint8_t Track{};
        double Average{};
        std::uint8_t RaiseCeiling{};
        std::int32_t Count{};
        std::uint8_t Complete = true;
        for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
            if (InventionLevels[Track] < 8) {
                Complete = false;
            }
        }
        if (Complete) {
            return;
        }
        double Progress = CalculateInventionProgressRate();
        Progress = static_cast<long double>(Progress) * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[4]].InventionProgressScale;
        CurrentInventionPoints = static_cast<long double>(CurrentInventionPoints) + Progress;
        if (CurrentInventionPoints > 1.0E+2L) {
            InventionLevels[CurrentInvention] = std::min<std::int32_t>(8, InventionLevels[CurrentInvention] + 1);
            CurrentInventionPoints = 0.0f;
            Complete = true;
            for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
                if (InventionLevels[Track] < 8) {
                    Complete = false;
                }
            }
            if (Complete) {
                return;
            }
            RaiseCeiling = true;
            do {
                for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
                    if (ResearchLevelPercent > System::Round(InventionLevels[Track] * 12.5L) && aConst::PlanetInventionInfo[Track].RequiredMainTechLevel <= InventionLevels[7] && InventionLevels[Track] <= InventionLevels[7]) {
                        RaiseCeiling = false;
                    }
                }
                if (!RaiseCeiling) {
                    Average = 0.0;
                    Count = 0;
                    for (Track = static_cast<std::uint8_t>(0); Track <= static_cast<std::uint8_t>(19); ++Track) {
                        if (aConst::PlanetInventionInfo[Track].RequiredMainTechLevel <= InventionLevels[7]) {
                            Average = Average + InventionLevels[Track] * 12.5L;
                            ++Count;
                        }
                    }
                    if (Count == 0) {
                        Count = 1;
                    }
                    Average = pas::real_divide(Average, Count);
                    if (static_cast<long double>(ResearchLevelPercent) < Average) {
                        RaiseCeiling = true;
                    }
                }
                if (RaiseCeiling) {
                    if (ResearchLevelPercent + ResearchLevelStep < 100) {
                        ResearchLevelPercent = ResearchLevelPercent + ResearchLevelStep;
                    } else {
                        ResearchLevelPercent = 100;
                    }
                }
            } while (RaiseCeiling);
            SelectCurrentInvention();
        }
    }

    // Radius factor times economy and race multipliers; excludes the difficulty multiplier.
    float TPlanet::CalculateInventionProgressRate() {
        return aMyFunction::RemapClamped(Radius, 6.0E+1, 1.0E+2, 0.7, 1.3) * (static_cast<long double>(aConst::PlanetEconomyInfo[Economy].InventionProgressScale) * aConst::PlanetRaceMarket[RaceId].InventionProgressScale);
    }

    void* TPlanet::BuyRanger(std::int32_t MoneyPercent) {
        std::int32_t Budget{};
        aRanger::TRanger* Ranger = pas::construct_call<aRanger::TRanger>(aNormalShip::TNormalShip_Create);
        ++HomeRangerCount;
        if (aGalaxy::Galaxy->CurrentTurn < 300) {
            Budget = aGalaxy::Galaxy->MaxRangerWealth;
        } else {
            std::int64_t averageRangerCapital = static_cast<std::int64_t>(aGalaxy::Galaxy->AverageRangerCapital);
            pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.4, 0.6);
            std::int64_t round = System::Round(cpp_left * aGalaxy::Galaxy->MaxRangerWealth);
            Budget = std::min<std::int64_t>(averageRangerCapital, round);
        }
        if (Budget > 500000) {
            Budget = 500000;
        }
        Budget = System::Round(Budget * 0.01L * MoneyPercent);
        Ranger->InitializeAtPlanet(this, Budget);
        void* Result = Ranger;
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    // Kind 0 randomizes the subtype, 3 selects transport, 4 liner, and all others diplomat. Result is owned by CurrentStar.Ships.
    void* TPlanet::SpawnTransport(std::uint8_t Kind, std::int32_t MoneyPercent) {
        aTransport::TTransportType SubType{};
        aTransport::TTransport* Transport = pas::construct_call<aTransport::TTransport>(aNormalShip::TNormalShip_Create);
        pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.2, 0.4);
        std::int32_t Budget = System::Round(cpp_left * aGalaxy::Galaxy->MaxRangerWealth);
        if (Budget > 600000) {
            Budget = 600000;
        }
        Budget = System::Round(Budget * 0.01L * MoneyPercent);
        if (Kind == 0) {
            aTransport::TTransport_InitGenerated(Transport, this, Budget, aTransport::ttTransport, true);
        } else {
            if (Kind == 3) {
                SubType = aTransport::ttTransport;
            } else if (Kind == 4) {
                SubType = aTransport::ttLiner;
            } else {
                SubType = aTransport::ttDiplomat;
            }
            aTransport::TTransport_InitGenerated(Transport, this, Budget, SubType, false);
        }
        void* Result = Transport;
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    void* TPlanet::BuyPirate(std::int32_t MoneyPercent) {
        aPirate::TPirate* Pirate = pas::construct_call<aPirate::TPirate>(aNormalShip::TNormalShip_Create);
        pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.3, 0.5);
        std::int32_t Budget = System::Round(cpp_left * aGalaxy::Galaxy->MaxRangerWealth);
        if (Budget > 800000) {
            Budget = 800000;
        }
        Budget = System::Round(Budget * 0.01L * MoneyPercent);
        Pirate->InitGenerated(this, Budget, 0);
        void* Result = Pirate;
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    // Creates an unowned Tranclucator docked here and inserts it into CurrentStar.Ships.
    void* TPlanet::SpawnTranclucator(std::uint8_t BasicEquipment) {
        aTranclucator::TTranclucator* Ship = pas::construct_call<aTranclucator::TTranclucator>(aTranclucator::TTranclucator_Create);
        Ship->Init(nullptr, OwnerId, BasicEquipment);
        Ship->CurrentPlanet = this;
        Ship->CurrentStar = CurrentStar;
        pas::list_add(CurrentStar->Ships, reinterpret_cast<void*>(Ship));
        return Ship;
    }

    // Creates a TPirate for Pirate Clan ownership, otherwise a TWarrior.
    void* TPlanet::BuyWarrior(std::int32_t MoneyPercent) {
        void* Result{};
        std::int32_t Budget{};
        aWarrior::TWarrior* Warrior{};
        aPirate::TPirate* Pirate{};
        std::int32_t Kind{};
        if (OwnerId == aGalaxyStruct::oiPirate) {
            Pirate = pas::construct_call<aPirate::TPirate>(aNormalShip::TNormalShip_Create);
            {
                pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.3, 0.5);
                Budget = System::Round(cpp_left * aGalaxy::Galaxy->MaxRangerWealth);
            }
            if (Budget > 800000) {
                Budget = 800000;
            }
            {
                std::int32_t cpp_left_2 = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
                Budget = System::Round(aMyFunction::RemapClamped(cpp_left_2 + aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition), 0.0, 1.0E+2, Budget * 0.7L, Budget * 1.2L));
            }
            if (aMyFunction::NextRandomUnitFloat(RandomState) > 0.2L) {
                Budget = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[2], -5.0, 5.0, Budget * 2, Budget * 0.5L));
            }
            Budget = System::Round(Budget * 0.01L * MoneyPercent);
            Kind = aMyFunction::NextRandomIntRange(0, 9, RandomState);
            if (Kind <= 3) {
                Pirate->InitGenerated(this, Budget, 1);
            } else if (Kind <= 7) {
                Pirate->InitGenerated(this, Budget, 2);
            } else {
                Pirate->InitGenerated(this, Budget, 3);
            }
            Result = Pirate;
        } else {
            Warrior = pas::construct_call<aWarrior::TWarrior>(aNormalShip::TNormalShip_Create);
            {
                pas::Extended cpp_left_3 = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.3, 0.5);
                Budget = System::Round(cpp_left_3 * aGalaxy::Galaxy->MaxRangerWealth);
            }
            if (Budget > 900000) {
                Budget = 900000;
            }
            Budget = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition), 0.0, 1.0E+2, Budget * 1.2L, Budget * 0.7L));
            if (aMyFunction::NextRandomUnitFloat(RandomState) > 0.2L) {
                Budget = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[0], -5.0, 5.0, Budget * 2, Budget * 0.5L));
            }
            Budget = System::Round(Budget * 0.01L * MoneyPercent);
            Warrior->InitGenerated(this, Budget, aWarrior::wtRegular);
            Result = Warrior;
        }
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    // Generates a warrior with WarriorType=1 and scales its budget by Coalition control.
    void* TPlanet::BuyFlagship(std::int32_t MoneyPercent) {
        aWarrior::TWarrior* Warrior = pas::construct_call<aWarrior::TWarrior>(aNormalShip::TNormalShip_Create);
        pas::Extended cpp_left = aMyFunction::RemapClamped(aMyFunction::NextRandomUnitFloat(RandomState), 0.0, 1.0, 0.3, 0.5);
        std::int32_t Budget = System::Round(cpp_left * aGalaxy::Galaxy->MaxRangerWealth);
        if (Budget > 900000) {
            Budget = 900000;
        }
        Budget = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition), 0.0, 1.0E+2, Budget * 1.2L, Budget * 0.7L));
        if (aMyFunction::NextRandomUnitFloat(RandomState) > 0.2L) {
            Budget = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->WarDeltaWin[0], -5.0, 5.0, Budget * 2, Budget * 0.5L));
        }
        Budget = System::Round(Budget * 0.01L * MoneyPercent);
        Warrior->InitGenerated(this, Budget, aWarrior::wtFlagship);
        void* Result = Warrior;
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    // Excludes boss type; suppresses Bertors when the constellation already has one of this series or Self is the spawn proxy.
    void* TPlanet::SpawnWeightedDominatorShip() {
        aGalaxyStruct::TKlingType Kind{};
        std::int32_t Level = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfDominators), 0.0, 1.0E+2, 5.0, 1.0));
        std::int32_t Total = 0;
        aGalaxyStruct::TKlingType LargestKind = aGalaxyStruct::ktBoss;
        std::int32_t LargestWeight = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range.next(Kind); ) {
            if (Kind == aGalaxyStruct::ktBoss) {
                continue;
            }
            if (Kind == aGalaxyStruct::ktBertor && (CurrentStar->Constellation->HasBertorOfSeries(CurrentStar->Status.DominatorSeries) || aKling::DominatorSpawnPlanet == this)) {
                continue;
            }
            Total += DominatorSpawnWeights[Level][Kind];
            if (DominatorSpawnWeights[Level][Kind] > LargestWeight || LargestKind == aGalaxyStruct::ktBoss) {
                LargestWeight = DominatorSpawnWeights[Level][Kind];
                LargestKind = Kind;
            }
        }
        std::int32_t Accumulated = 0;
        std::int32_t Roll = aMyFunction::NextRandomIntRange(0, Total, RandomState);
        for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_2.next(Kind); ) {
            if (Kind == aGalaxyStruct::ktBoss) {
                continue;
            }
            if (Kind == aGalaxyStruct::ktBertor && (CurrentStar->Constellation->HasBertorOfSeries(CurrentStar->Status.DominatorSeries) || aKling::DominatorSpawnPlanet == this)) {
                continue;
            }
            Accumulated += DominatorSpawnWeights[Level][Kind];
            if (Roll <= Accumulated) {
                return SpawnDominatorShip(Kind);
            }
        }
        return SpawnDominatorShip(aGalaxyStruct::ktShtip);
    }

    void* TPlanet::SpawnDominatorShip(aGalaxyStruct::TKlingType Kind) {
        aKling::TKling* Ship = pas::construct_call<aKling::TKling>(aShip::TShip_Create);
        aGalaxyStruct::TDominatorSeries Series = CurrentStar->Status.DominatorSeries;
        if (CurrentStar->Status.ControlFaction != aGalaxyStruct::sfDominators) {
            if (Series == aGalaxyStruct::dsTerron && aKling::TerronShip != nullptr && aGalaxy::Galaxy->TerronToStarTurn >= 0x40000000) {
                Series = aGalaxyStruct::dsKeller;
            }
            if (Series == aGalaxyStruct::dsBlazer && aGalaxy::Galaxy->BlazerSelfDestructTurn != 0) {
                Series = aGalaxyStruct::dsKeller;
            }
        }
        Ship->InitGenerated(Kind, this, Series);
        void* Result = Ship;
        CurrentStar->DaysSinceLastNpcShipSpawn = 0;
        return Result;
    }

    // Uses group owner/type/equipment constraints; returns nil for a boss request. Temporarily changes planet ownership, star series and global technology.
    void* TPlanet::GenerateShipForScriptGroup(void* Group) {
        aGalaxyStruct::TOwnerId Owner{};
        aGalaxyStruct::TOwnerId SelectedOwner{};
        aGalaxyStruct::TDominatorSeries Series{};
        aGalaxyStruct::TKlingType Kind{};
        std::uint8_t ShipKind{};
        std::int32_t j{};
        std::int32_t Count{};
        pas::Object* Rules = pas::checked_cast<aScript::TScriptGroup*>(static_cast<pas::Object*>(Group));
        if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->OwnerMask, OwnerId)) {
            SelectedOwner = OwnerId;
        } else {
            SelectedOwner = aGalaxyStruct::oiMaloc;
            Count = 0;
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range.next(Owner); ) {
                if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->OwnerMask, Owner)) {
                    ++Count;
                }
            }
            Count = aMyFunction::NextRandomIntRange(1, Count, RandomState);
            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiPirate); cpp_range_2.next(Owner); ) {
                if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->OwnerMask, Owner)) {
                    --Count;
                    SelectedOwner = Owner;
                    if (Count <= 0) {
                        break;
                    }
                }
            }
        }
        std::uint8_t SelectedShipKind = 0;
        aGalaxyStruct::TKlingType SelectedKind = aGalaxyStruct::ktBoss;
        aGalaxyStruct::TDominatorSeries SelectedSeries = aGalaxyStruct::dsBlazer;
        Count = 0;
        for (ShipKind = static_cast<std::uint8_t>(0); ShipKind <= static_cast<std::uint8_t>(10); ++ShipKind) {
            if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->ShipTypeMask, ShipKind)) {
                if (ShipKind == 6) {
                    for (auto cpp_range_3 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_3.next(Kind); ) {
                        for (auto cpp_range_4 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_4.next(Series); ) {
                            if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->DominatorMasks[Kind], static_cast<std::uint8_t>(Series))) {
                                ++Count;
                            }
                        }
                    }
                } else {
                    ++Count;
                }
            }
        }
        Count = aMyFunction::NextRandomIntRange(1, Count, RandomState);
        std::uint8_t Found = false;
        for (ShipKind = static_cast<std::uint8_t>(0); ShipKind <= static_cast<std::uint8_t>(10); ++ShipKind) {
            if (!pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->ShipTypeMask, ShipKind)) {
                continue;
            }
            if (ShipKind == 6) {
                for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_5.next(Kind); ) {
                    for (auto cpp_range_6 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_6.next(Series); ) {
                        if (pas::contains(reinterpret_cast<aScript::TScriptGroup*>(Rules)->DominatorMasks[Kind], static_cast<std::uint8_t>(Series))) {
                            --Count;
                            SelectedKind = Kind;
                            SelectedShipKind = ShipKind;
                            SelectedSeries = Series;
                            Found = Count <= 0;
                        }
                        if (Found) {
                            break;
                        }
                    }
                    if (Found) {
                        break;
                    }
                }
            } else {
                --Count;
                SelectedShipKind = ShipKind;
                Found = Count <= 0;
            }
            if (Found) {
                break;
            }
        }
        void* Result = nullptr;
        if (SelectedShipKind == 6 && SelectedKind == aGalaxyStruct::ktBoss) {
            return Result;
        }
        aGalaxyStruct::TOwnerId OldOwner = OwnerId;
        OwnerId = SelectedOwner;
        aGalaxyStruct::TOwnerId OldRace = RaceId;
        if (pas::in_range(OwnerId, static_cast<std::int32_t>(aGalaxyStruct::oiMaloc), static_cast<std::int32_t>(aGalaxyStruct::oiGaal))) {
            RaceId = aConst::OwnerToRace(OwnerId);
        }
        aGalaxyStruct::TDominatorSeries OldSeries = CurrentStar->Status.DominatorSeries;
        CurrentStar->Status.DominatorSeries = SelectedSeries;
        std::int32_t OldTechLevel = aGalaxy::Galaxy->TechLevel;
        std::int32_t i = 0;
        aShip::TShip* Ship = nullptr;
        while (Ship == nullptr) {
            {
                std::uint8_t cpp_case = SelectedShipKind;
                if (cpp_case == 0) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(BuyRanger(100)));
                } else if (cpp_case == 1) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(BuyWarrior(100)));
                } else if (cpp_case == 2) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(BuyPirate(100)));
                } else if (cpp_case >= 3 && cpp_case <= 5) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(SpawnTransport(SelectedShipKind, 100)));
                } else if (cpp_case == 6) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(SpawnDominatorShip(SelectedKind)));
                } else if (cpp_case == 7) {
                    Ship = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(SpawnTranclucator(true)));
                }
            }
            ++i;
            if (i % 50 == 0) {
                aGalaxy::Galaxy->TechLevel = std::min<std::int32_t>(8, aGalaxy::Galaxy->TechLevel + 1);
            }
            if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinCargoHookLevel > 0) {
                if (Ship->GetSlotCount(aConst::sskCargoHook) < 1) {
                    pas::free(Ship);
                    Ship = nullptr;
                }
            }
        }
        aGalaxy::Galaxy->TechLevel = OldTechLevel;
        if (Ship != nullptr) {
            i = 0;
            do {
                Money += aGalaxy::Galaxy->ComputeScaledSmallMoney(OwnerId);
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinCargoHookLevel > 0) {
                    if (Ship->GetCargoHook() != nullptr && Ship->GetCargoHook()->TechLevel >= reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinCargoHookLevel) {
                        aShip::TShip_LiquidateInventoryItem(Ship, Ship->GetCargoHook());
                    }
                    Ship->CreateAndEquipCargoHook(System::Round(aConst::CargoHookBaseSize), reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinCargoHookLevel, OwnerId);
                    Ship->RefreshDerivedStats(true);
                }
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinSpeed > Ship->Speed) {
                    Ship->ImproveRandomEquipment(true);
                }
                Ship->RefreshDerivedStats(true);
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinStrength > Ship->StrengthInBestRanger && reinterpret_cast<aScript::TScriptGroup*>(Rules)->WeaponRequirement == 1) {
                    Ship->ImproveRandomEquipment(true);
                }
                Ship->RefreshDerivedStats(true);
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinStrength > Ship->StrengthInBestRanger && reinterpret_cast<aScript::TScriptGroup*>(Rules)->WeaponRequirement == 1) {
                    Ship->ImproveRandomEquipment(true);
                }
                Ship->RefreshDerivedStats(true);
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxStrength < Ship->StrengthInBestRanger && reinterpret_cast<aScript::TScriptGroup*>(Rules)->WeaponRequirement == 1) {
                    if (Ship->CountEquippedWeapons() > 1) {
                        aShip::TShip_LiquidateInventoryItem(Ship, Ship->Weapons[1]);
                    }
                }
                Ship->RefreshDerivedStats(true);
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxStrength < Ship->StrengthInBestRanger && reinterpret_cast<aScript::TScriptGroup*>(Rules)->WeaponRequirement == 1) {
                    if (Ship->GetDefGenerator() != nullptr) {
                        aShip::TShip_LiquidateInventoryItem(Ship, Ship->GetDefGenerator());
                    }
                }
                if (reinterpret_cast<aScript::TScriptGroup*>(Rules)->WeaponRequirement == 2 && Ship->WeaponCount > 0) {
                    const std::int32_t cpp_first = static_cast<std::int32_t>(Ship->WeaponCount);
                    if (cpp_first >= 1) {
                        for (j = cpp_first; j >= 1; --j) {
                            aShip::TShip_LiquidateInventoryItem(Ship, Ship->Weapons[j]);
                        }
                    }
                }
                ++i;
            } while (!(i == 11));
        }
        if (Ship != nullptr) {
            if (Ship->GetCargoFreeSpace() < reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinFreeCargoSpace) {
                Ship->GetHull()->Weight += reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinFreeCargoSpace - Ship->CargoFreeSpace;
                Ship->GetHull()->HullPoints = Ship->GetHull()->Weight;
                Ship->RefreshDerivedStats(true);
            }
        }
        if (Ship != nullptr && pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
            // Native code complements the Byte before testing the range. Preserve
            // that behavior rather than interpreting it as a negated membership test.
            if (pas::contains(pas::make_set<pas::Set<0, 255>>({{reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinTraderStatus, reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxTraderStatus}}), ~static_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader])) {
                static_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] = (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinTraderStatus + reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxTraderStatus) / 2;
                pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] = (100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader]) / 2;
                {
                    std::int32_t cpp_left = 100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader];
                    pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] = cpp_left - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate];
                }
            }
            if (pas::contains(pas::make_set<pas::Set<0, 255>>({{reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinPirateStatus, reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxPirateStatus}}), ~pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate])) {
                static_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] = (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinPirateStatus + reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxPirateStatus) / 2;
                pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] = (100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate]) / 2;
                {
                    std::int32_t cpp_left_2 = 100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate];
                    pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] = cpp_left_2 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader];
                }
            }
            if (pas::contains(pas::make_set<pas::Set<0, 255>>({{reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinWarriorStatus, reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxWarriorStatus}}), ~pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior])) {
                static_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior] = (reinterpret_cast<aScript::TScriptGroup*>(Rules)->MinWarriorStatus + reinterpret_cast<aScript::TScriptGroup*>(Rules)->MaxWarriorStatus) / 2;
                pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader] = (100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior]) / 2;
                {
                    std::int32_t cpp_left_3 = 100 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcWarrior];
                    pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcPirate] = cpp_left_3 - pas::checked_cast<aRanger::TRanger*>(Ship)->CareerStatus[aGalaxyStruct::rcTrader];
                }
            }
        }
        Ship->RefreshDerivedStats(true);
        if (OwnerId != OldOwner || RaceId != OldRace) {
            RaceId = OldRace;
            OwnerId = OldOwner;
            Ship->HomePlanet = pas::checked_cast<TPlanet*>(static_cast<pas::Object*>(FindNearestPlanetByOwnerMask(pas::make_set<aGalaxyStruct::TOwnerMask>({{static_cast<std::int32_t>(Ship->OwnerId)}}))));
            if (Ship->HomePlanet == nullptr) {
                Ship->HomePlanet = this;
            }
            if (pas::class_cast_if<aWarrior::TWarrior*>(Ship) != nullptr) {
                i = pas::list_indexof(Warriors, reinterpret_cast<void*>(Ship));
                if (i >= 0) {
                    pas::list_delete(Warriors, i);
                }
                i = pas::list_indexof(Ship->HomePlanet->Warriors, reinterpret_cast<void*>(Ship));
                if (i < 0) {
                    pas::list_add(Ship->HomePlanet->Warriors, reinterpret_cast<void*>(Ship));
                }
            }
        }
        CurrentStar->Status.DominatorSeries = OldSeries;
        return Ship;
    }

    std::int32_t TPlanet::RelationToRanger(std::int32_t RangerIndex) {
        std::int32_t Result{};
        try {
            if (OwnerId == aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) && MainPiratePlanet != nullptr) {
                Result = MainPiratePlanet->RelationToRanger(RangerIndex);
            } else if (IsMainPiratePlanet && OwnerId != aGalaxyStruct::oiPirate) {
                Result = 50;
            } else {
                Result = static_cast<aGalaxyStruct::TPercent>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, RangerIndex))));
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TPlanet.RelationToRanger ", Name, u" planet owner = ", pas::wide_int_to_str(static_cast<std::int32_t>(OwnerId)), u" i = ", pas::wide_int_to_str(RangerIndex), u" count = ", pas::wide_int_to_str(pas::list_count(RangerRelations))}))));
            } else {
                throw;
            }
        }
        return Result;
    }

    void TPlanet::SetRelationLevelToRanger(void* Ranger, aGalaxyStruct::TRelationLevel Level) {
        switch (Level) {
            case aGalaxyStruct::rlHostile: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(5))));
                break;
            }
            case aGalaxyStruct::rlBad: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(20))));
                break;
            }
            case aGalaxyStruct::rlNormal: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(45))));
                break;
            }
            case aGalaxyStruct::rlGood: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(70))));
                break;
            }
            case aGalaxyStruct::rlExcellent: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(90))));
                break;
            }
            default: {
                pas::list_put(RangerRelations, pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger)))), reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(5))));
                break;
            }
        }
    }

    void TPlanet::ChangeRelationToRanger(void* Ranger, std::int32_t Amount) {
        std::int32_t Index = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))));
        std::uint8_t Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index)));
        if (pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) > 0 && Amount > 0) {
            Amount += System::Round(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ranger))->GetEffectiveSkillLevel(aGalaxyStruct::psCharisma, false) * Amount * 0.2L);
        }
        if (pas::in_range(Relation + Amount, 0, 100)) {
            Relation += Amount;
        } else if (Relation + Amount > 100) {
            Relation = 100;
        } else {
            Relation = 0;
        }
        pas::list_put(RangerRelations, Index, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Relation))));
        if (aPlayer::GetPlayer() == Ranger && GetRelationLevelToShip(Ranger) <= aGalaxyStruct::rlHostile) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckHaterAchievement();
        }
        if (aPlayer::GetPlayer() == Ranger) {
            if (aMyFunction::RandomIntRange(0, 100) == 0) {
                SysUtilsImports::Sleep(1u);
            }
            if (static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(RangerRelations, Index))) != Relation && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
                GR_Main::CCInterface->SetTamperDetected(true);
            }
        }
    }

    aGalaxyStruct::TPercent TPlanet::RelationToShip(void* Ship) {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->PirateRank == 7 && CurrentStar->Constellation->Id == 20 && static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) && OwnerId == aGalaxyStruct::oiPirate) {
            return 100;
        }
        if (OwnerId == aGalaxyStruct::oiUninhabited) {
            return 100;
        }
        aGalaxyStruct::TPercent Result = 0;
        if (CurrentStar->Status.CustomFaction != u"") {
            if (static_cast<aShip::TShip*>(Ship)->ScriptShip != nullptr) {
                if (reinterpret_cast<aScript::TScriptShip*>(static_cast<aShip::TShip*>(Ship)->ScriptShip)->StateText == CurrentStar->Status.CustomFaction) {
                    return 100;
                }
            }
            return Result;
        }
        if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(OwnerId)) {
            if (pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->CurrentStar == CurrentStar && (static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) || aPlayer::GetPlayer() != Ship)) {
                if (pas::contains(aConst::PlanetOwnerMasks.PirateClan, OwnerId) && pas::is_one_of<aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive>(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->CurrentStanding)) {
                    return Result;
                }
                if (pas::contains(aConst::PlanetOwnerMasks.Coalition, OwnerId) && pas::is_one_of<aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary>(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->CurrentStanding)) {
                    return Result;
                }
            }
            {
                std::uint8_t cpp_case = pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->TypeId;
                if (cpp_case == aGalaxyStruct::stRanger) {
                    return RelationToRanger(pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(pas::checked_cast<aRanger::TRanger*>(static_cast<pas::Object*>(Ship)))));
                } else if (cpp_case == aGalaxyStruct::stTransport) {
                    if (pas::contains(aConst::PlanetOwnerMasks.PirateClan, OwnerId)) {
                        return std::min<std::int32_t>(50, static_cast<std::int32_t>(aConst::OwnerRelations[OwnerId][pas::checked_cast<aTransport::TTransport*>(static_cast<pas::Object*>(Ship))->OwnerId]));
                    }
                    return aConst::OwnerRelations[OwnerId][pas::checked_cast<aTransport::TTransport*>(static_cast<pas::Object*>(Ship))->OwnerId];
                } else if (cpp_case == aGalaxyStruct::stPirate) {
                    if (pas::contains(aConst::PlanetOwnerMasks.PirateClan, OwnerId)) {
                        if (static_cast<aShip::TShip*>(Ship)->OwnerId == aGalaxyStruct::oiPirate) {
                            return 100;
                        }
                        return aConst::OwnerRelations[OwnerId][static_cast<aShip::TShip*>(Ship)->OwnerId];
                    }
                    return std::max<std::int64_t>(static_cast<std::int64_t>(30), std::min<std::int64_t>(static_cast<std::int64_t>(aConst::PlanetRaceMarket[RaceId].PirateRelationCeiling), System::Round(static_cast<long double>(aConst::OwnerRelations[OwnerId][pas::checked_cast<aPirate::TPirate*>(static_cast<pas::Object*>(Ship))->OwnerId]) * aConst::PlanetRaceMarket[RaceId].PirateRelationFactor)));
                } else if (cpp_case == aGalaxyStruct::stWarrior) {
                    if (pas::contains(aConst::PlanetOwnerMasks.PirateClan, OwnerId)) {
                        return 0;
                    }
                    return 100;
                } else if (cpp_case == aGalaxyStruct::stKling) {
                    return 0;
                } else if (cpp_case == aGalaxyStruct::stTranclucator) {
                    if (pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship))->OwnerShip != nullptr) {
                        return RelationToShip(pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(Ship))->OwnerShip);
                    }
                    return 50;
                } else if (cpp_case >= aGalaxyStruct::rstRangerCenter && cpp_case <= aGalaxyStruct::rstCustomStation) {
                    if (pas::contains(aConst::FactionStandingMasks[CurrentStar->Status.ControlFaction], pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->CurrentStanding)) {
                        return 100;
                    }
                    return 0;
                } else {
                    return 50;
                }
            }
        } else if (OwnerId == aGalaxyStruct::oiDominator) {
            if (pas::in_range(pas::checked_cast<aShip::TShip*>(static_cast<pas::Object*>(Ship))->TypeId, aGalaxyStruct::stKling, aGalaxyStruct::stKling)) {
                return 100;
            }
            return 0;
        } else {
            return Result;
        }
    }

    aGalaxyStruct::TRelationLevel TPlanet::GetRelationLevelToShip(void* Ship) {
        aGalaxyStruct::TRelationLevel Result{};
        {
            aGalaxyStruct::TPercent cpp_case = RelationToShip(Ship);
            if (cpp_case >= 0 && cpp_case <= 9) {
                Result = aGalaxyStruct::rlHostile;
            } else if (cpp_case >= 10 && cpp_case <= 29) {
                Result = aGalaxyStruct::rlBad;
            } else if (cpp_case >= 30 && cpp_case <= 59) {
                Result = aGalaxyStruct::rlNormal;
            } else if (cpp_case >= 60 && cpp_case <= 79) {
                Result = aGalaxyStruct::rlGood;
            } else if (cpp_case >= 80 && cpp_case <= 100) {
                Result = aGalaxyStruct::rlExcellent;
            } else {
                Result = aGalaxyStruct::rlNormal;
            }
        }
        if (IsMainPiratePlanet && aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->CurrentPlanet == this && Result == aGalaxyStruct::rlHostile) {
            return aGalaxyStruct::rlBad;
        }
        return Result;
    }

    pas::WideString TPlanet::GetRelationLevelTextToShip(void* Ship) {
        return aConst::RelationInfo[GetRelationLevelToShip(Ship)].DisplayName;
    }

    pas::WideString TPlanet::GetCivilInfoText() {
        pas::WideString Text{};
        Text = aConst::LocalizedText(u"Planet.Civil.Info.TextAboutPlanet"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, CurrentStar->Name, u"<color=255,240,100>"_w);
        if (IsMainPiratePlanet) {
            aMyFunction::ReplaceTextToken(Text, u"<Race>"_w, aConst::OwnerInfo[OwnerId].DisplayName, u"<color=255,240,100>"_w);
        } else {
            aMyFunction::ReplaceTextToken(Text, u"<Race>"_w, GetNativeRaceName(), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(Text, u"<Population>"_w, pas::wide_int64_to_str(System::Round(pas::real_divide(Population, 1.0E+3L))), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Economy>"_w, aConst::PlanetEconomyInfo[Economy].DisplayName, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Goverment>"_w, GetGovernmentName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Relation>"_w, GetRelationLevelTextToShip(aPlayer::GetPlayer()), u"<color=255,240,100>"_w);
        if (GetRelationLevelToShip(aPlayer::GetPlayer()) <= aGalaxyStruct::rlBad && static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1)) {
            Text = pas::concat_wide({Text, u"\r\n", aConst::LocalizedText(u"Planet.Civil.Info.BadDopInfo"_wref.get())});
        }
        return Text;
    }

    // Tests normal-space ships in CurrentStar for RelationToShip < 10.
    std::uint8_t TPlanet::HasHostileShipsInSystem() {
        std::int32_t i{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(i); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, i);
            if (Ship->InNormalSpace()) {
                if (RelationToShip(Ship) < 10) {
                    return true;
                }
            }
        }
        return false;
    }

    // Returns a zero-based module index or -1; advances planet RNG.
    std::int32_t TPlanet::SelectEquipmentOfferSpecialMicroModule(aItem::TEquipment* Item) {
        std::int32_t i{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        if (!IsMainPiratePlanet) {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(i); ) {
            if (Template->SpecialOnly && (static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) || pas::constant_set<pas::Set<0, 255>>({{7}, {12}}) * static_cast<pas::Set<0, 255>>(Template->OfferStationTypes) != pas::constant_set<pas::Set<0, 255>>({})) && (IsMainPiratePlanet || Template->OnPlanets) && aItem::IsBonusCompatibleWithEquipment(i, Item) && Template->Priority <= Ceiling) {
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = i;
                ++Count;
            }
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
        }
        std::int32_t Selected = -1;
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (i = 0; i <= 10; ++i) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Selected = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Selected;
    }

    // Returns a zero-based module index or -1; advances planet RNG.
    std::int32_t TPlanet::SelectHullOfferSpecialMicroModule(aItem::THull* Hull) {
        std::int32_t i{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        if (!IsMainPiratePlanet) {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(i); ) {
            if (Template->SpecialOnly && (static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) || pas::constant_set<pas::Set<0, 255>>({{7}, {12}}) * static_cast<pas::Set<0, 255>>(Template->OfferStationTypes) != pas::constant_set<pas::Set<0, 255>>({})) && (IsMainPiratePlanet || Template->OnPlanets) && aItem::IsBonusCompatibleWithHull(i, Hull) && Template->Priority <= Ceiling) {
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = i;
                ++Count;
            }
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
        }
        std::int32_t Selected = -1;
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (i = 0; i <= 10; ++i) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Selected = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Selected;
    }

    // Returns a zero-based module index or -1; advances planet RNG.
    std::int32_t TPlanet::SelectWeaponOfferSpecialMicroModule(aItem::TWeapon* Weapon) {
        std::int32_t i{};
        std::int32_t Candidate{};
        std::int32_t Result = -1;
        if (!IsMainPiratePlanet) {
            pas::Extended cpp_left = aMyFunction::NextRandomIntRange(1, 100, RandomState);
            if (cpp_left > aGalaxy::Galaxy->GetMicroModuleOfferRollThresholdPercent()) {
                return Result;
            }
        }
        std::int32_t Ceiling = System::Round(pas::real_divide(InventionLevels[7] * 100, 8.0L));
        std::int32_t Minimum = 0;
        std::int32_t Maximum = 0;
        std::int32_t Count = 0;
        aConst::PMicroModuleTemplate Template = static_cast<aConst::PMicroModuleTemplate>(aConst::MicroModuleTemplates.data());
        for (auto cpp_range = pas::for_to<std::int32_t>(0, aConst::MicroModuleTemplateCount - 1); cpp_range.next(i); ) {
            if (Template->SpecialOnly && (static_cast<std::uint8_t>(IsMainPiratePlanet ^ 1) || pas::constant_set<pas::Set<0, 255>>({{7}, {12}}) * static_cast<pas::Set<0, 255>>(Template->OfferStationTypes) != pas::constant_set<pas::Set<0, 255>>({})) && (IsMainPiratePlanet || Template->OnPlanets) && aItem::IsBonusCompatibleWithWeapon(i, Weapon) && Template->Priority <= Ceiling) {
                if (Count == 0) {
                    Minimum = Template->Priority;
                    Maximum = Template->Priority;
                } else {
                    Minimum = std::min<std::int32_t>(Minimum, static_cast<std::int32_t>(Template->Priority));
                    Maximum = std::max<std::int32_t>(Maximum, static_cast<std::int32_t>(Template->Priority));
                }
                aConst::MicroModuleCandidateIndices[Count] = i;
                ++Count;
            }
            Template = static_cast<aConst::PMicroModuleTemplate>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(Template) + static_cast<std::int32_t>(sizeof(aConst::TMicroModuleInfo))));
        }
        std::int32_t Selected = -1;
        if (Count > 0) {
            Minimum = std::max<std::int32_t>(0, Maximum - 40);
            for (i = 0; i <= 10; ++i) {
                Candidate = aMyFunction::NextRandomIntRange(0, Count - 1, RandomState);
                if (aConst::MicroModuleTemplates[aConst::MicroModuleCandidateIndices[Candidate]].Priority >= Minimum) {
                    Selected = aConst::MicroModuleCandidateIndices[Candidate];
                    break;
                }
            }
        }
        return Selected;
    }

    // Weekly replacement/generation gate; disabled by sumDisabled and sumGoodsOnly.
    void TPlanet::RefreshEquipmentShopInventory() {
        std::int32_t Index{};
        std::int32_t Attempts{};
        aItem::TEquipment* Item{};
        aConst::TItemType ItemType{};
        if (pas::is_one_of<aGalaxyStruct::sumDisabled, aGalaxyStruct::sumGoodsOnly>(static_cast<aGalaxyStruct::TShopUpdateMode>(ShopUpdateMode))) {
            return;
        }
        if ((aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(GenerationSeed)) % 7 == 0) {
            {
                std::int32_t cpp_left = CalculateEquipmentShopTargetCount();
                if (cpp_left <= pas::list_count(EquipmentShop)) {
                    if (aMyFunction::SeededRandomUnitFloat(aMyFunction::StepRandomSeed(aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(GenerationSeed) + 17)) < 0.5L || IsMainPiratePlanet) {
                        Index = aMyFunction::SeededRandomIntRange(0, pas::list_count(EquipmentShop) - 1, aGalaxy::Galaxy->CurrentTurn * GenerationSeed);
                        Item = pas::list_at<aItem::TEquipment>(EquipmentShop, Index);
                        if (Item->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name == u"") {
                            pas::list_delete(EquipmentShop, Index);
                            pas::free(Item);
                        }
                    }
                }
            }
            if (([&] {
                std::int32_t cpp_left_2 = CalculateEquipmentShopTargetCount();
                return cpp_left_2 >= pas::list_count(EquipmentShop);
            }()) && aMyFunction::SeededRandomUnitFloat(aMyFunction::StepRandomSeed(aGalaxy::Galaxy->CurrentTurn + static_cast<std::int32_t>(GenerationSeed))) < 0.5L || aMyFunction::NextRandomIntRange(1, 100, RandomState) < 30 || IsMainPiratePlanet) {
                Attempts = 0;
                do {
                    ++Attempts;
                    ItemType = static_cast<aConst::TItemType>(aMyFunction::SeededRandomIntRange(42, 52, aGalaxy::Galaxy->CurrentTurn * GenerationSeed * 175 + Attempts));
                } while (!(Attempts > 30 || CountEquipmentShopItemsInBucket(ItemType) < aConst::PlanetEquipmentOfferQuotas[RaceId][ItemType - aConst::t_Hull]));
                Item = aPlanet::TPlanet_GenerateEquipmentOffer(this, aPlayer::GetPlayer(), ItemType);
                if (Item != nullptr) {
                    pas::list_add(EquipmentShop, reinterpret_cast<void*>(Item));
                    RemoveSimilarEquipmentShopItem(Item);
                }
            }
        }
    }

    aItem::THull* TPlanet::GenerateHullOffer(void* Ship) {
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        std::int32_t Size{};
        aItem::THull* Result = nullptr;
        if (Ship == nullptr || !(pas::class_cast_if<aShip::TShip*>(static_cast<pas::Object*>(Ship)) != nullptr)) {
            return Result;
        }
        aShip::TShip* Target = static_cast<aShip::TShip*>(Ship);
        std::uint8_t HullType = aGalaxyStruct::htRanger;
        std::int32_t Attempts = 0;
        while (true) {
            if (Attempts > 100) {
                break;
            }
            ++Attempts;
            HullType = aMyFunction::NextRandomIntRange(0, 5, RandomState);
            if (Government == aGalaxyStruct::pgAnarchy && pas::is_one_of<aGalaxyStruct::htWarrior, aGalaxyStruct::htDiplomat>(HullType)) {
                continue;
            }
            if (Government == aGalaxyStruct::pgDictatorship && pas::in_range(HullType, aGalaxyStruct::htLiner, aGalaxyStruct::htDiplomat)) {
                continue;
            }
            if (Government == aGalaxyStruct::pgRepublic && OwnerId != aGalaxyStruct::oiPeleng && aMyFunction::SeededRandomUnitFloat(RandomState) < 0.8L && pas::in_range(HullType, aGalaxyStruct::htPirate, aGalaxyStruct::htPirate)) {
                continue;
            }
            if (Government == aGalaxyStruct::pgDemocracy && OwnerId != aGalaxyStruct::oiPeleng && pas::in_range(HullType, aGalaxyStruct::htPirate, aGalaxyStruct::htPirate)) {
                continue;
            }
            if (OwnerId == aGalaxyStruct::oiPirate && HullType == aGalaxyStruct::htWarrior) {
                continue;
            }
            break;
        }
        std::uint8_t Flagship = pas::class_cast_if<aWarrior::TWarrior*>(Target) != nullptr && static_cast<aWarrior::TWarrior*>(Target)->WarriorType == aWarrior::wtFlagship;
        if (Flagship) {
            HullType = aGalaxyStruct::htFlagship;
        }
        if (aPlayer::GetPlayer() != Target && Target->GetHull()->HullType != aGalaxyStruct::htSpecial && Target->GetHull()->HullType != HullType) {
            return Result;
        }
        std::int32_t MaxLevel = InventionLevels[aConst::EquipmentInventionIndices[aConst::t_Hull]];
        std::int32_t MinLevel = std::max<std::int32_t>(1, MaxLevel / 2 - 1);
        switch (aGalaxy::Galaxy->GetHullGrowthMod()) {
            case 1: {
                Size = Target->GetHull()->EstimateCapacityWithoutBonuses();
                if (Flagship) {
                    Size = Size / 2;
                }
                MinSize = Size / 2;
                if (pas::in_range(HullType, aGalaxyStruct::htTransport, aGalaxyStruct::htLiner)) {
                    MaxSize = Size + aGalaxy::Galaxy->TechLevel * 40;
                } else if (pas::in_range(HullType, aGalaxyStruct::htDiplomat, aGalaxyStruct::htDiplomat)) {
                    MaxSize = Size + aGalaxy::Galaxy->TechLevel * 10;
                } else {
                    MaxSize = Size + aGalaxy::Galaxy->TechLevel * 25;
                }
                MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]));
                MaxSize = aGalaxy::Galaxy->ScaleIntByTechLevel(System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[4]), MaxSize);
                break;
            }
            case 2: {
                Size = Target->GetHull()->Weight;
                if (Flagship) {
                    Size = Size / 2;
                }
                MinSize = System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]);
                MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(Size), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(5, 1)]));
                break;
            }
            default: {
                Size = Target->GetHull()->Weight;
                if (Flagship) {
                    Size = Size / 2;
                }
                MinSize = Size / 2;
                if (pas::in_range(HullType, aGalaxyStruct::htTransport, aGalaxyStruct::htLiner)) {
                    MaxSize = Size + 300;
                } else if (pas::in_range(HullType, aGalaxyStruct::htDiplomat, aGalaxyStruct::htDiplomat)) {
                    MaxSize = Size + 50;
                } else {
                    MaxSize = Size + 200;
                }
                MinSize = std::max<std::int64_t>(static_cast<std::int64_t>(MinSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[5]));
                MaxSize = std::min<std::int64_t>(static_cast<std::int64_t>(MaxSize), System::Round(static_cast<long double>(aConst::HullBaseSize) * aConst::EquipmentSizeFactors[aGalaxy::Galaxy->ScaleIntByTechLevel(4, 1)]));
                break;
            }
        }
        aGalaxyStruct::TOwnerId Owner = aConst::RaceToOwner(RaceId);
        if (aMyFunction::NextRandomUnitFloat(RandomState) < 0.1L || IsMainPiratePlanet) {
            Owner = static_cast<aGalaxyStruct::TOwnerId>(aMyFunction::NextRandomIntRange(0, 4, RandomState));
        }
        if (aPlayer::GetPlayer() != Target && IsMainPiratePlanet) {
            Owner = Target->GetHull()->OwnerId;
        }
        if (Target->GetHull()->OwnerId != Owner && aPlayer::GetPlayer() != Target) {
            return Result;
        }
        Result = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
        std::int32_t Series = -1;
        std::int32_t ModuleIndex = -1;
        Result->OwnerId = Owner;
        Result->PirateBuilt = OwnerId == aGalaxyStruct::oiPirate;
        if (Target->CanGenerateSpecialHullModule()) {
            ModuleIndex = SelectHullOfferSpecialMicroModule(Result);
        }
        if (ModuleIndex < 0) {
            if (Target->CanGenerateSpecialHullModule()) {
                ModuleIndex = SelectHullOfferSpecialMicroModule(Result);
            }
            Series = aGalaxy::Galaxy->SelectHullSeries(Owner, HullType, 1, 100);
        }
        if (Flagship) {
            std::uint8_t cpp_arg = OwnerId == aGalaxyStruct::oiPirate;
            std::uint8_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
            std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize * 2, MaxSize * 2, RandomState);
            Result->Init(nextRandomIntRange_2, nextRandomIntRange, Owner, 10, Series, cpp_arg);
        } else {
            std::uint8_t cpp_arg_2 = OwnerId == aGalaxyStruct::oiPirate;
            std::uint8_t nextRandomIntRange_3 = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
            std::int32_t nextRandomIntRange_4 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, RandomState);
            Result->Init(nextRandomIntRange_4, nextRandomIntRange_3, Owner, HullType, Series, cpp_arg_2);
        }
        if (Target->GetHull()->SpecialModuleIndex > 0 && aPlayer::GetPlayer() != Target && ModuleIndex < 0) {
            ModuleIndex = Target->GetHull()->SpecialModuleIndex - 1;
        }
        if (ModuleIndex >= 0) {
            aItem::ApplySpecialMicroModule(ModuleIndex, Result);
        }
        return Result;
    }

    // New item or nil; does not add it to EquipmentShop.
    aItem::TWeapon* TPlanet::GenerateWeaponOffer(void* Ship) {
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        aGalaxyStruct::TWeaponAvailabilityMask Available{};
        aConst::PWeaponInfo Info{};
        std::int32_t ModuleIndex{};
        aGalaxyStruct::TOwnerId Owner{};
        aItem::TWeapon* Result = nullptr;
        if (Ship == nullptr || !(pas::class_cast_if<aShip::TShip*>(static_cast<pas::Object*>(Ship)) != nullptr)) {
            return Result;
        }
        aShip::TShip* Target = static_cast<aShip::TShip*>(Ship);
        Available = pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}});
        if (Target->TypeId == aGalaxyStruct::stKling && pas::contains(aConst::PlanetOwnerMasks.Dominators, OwnerId)) {
            Available = Available + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{4}});
        }
        if (pas::in_range(Target->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stWarrior) && pas::contains(aConst::PlanetOwnerMasks.Coalition, OwnerId)) {
            Available = Available + pas::constant_set<aGalaxyStruct::TWeaponAvailabilityMask>({{1}}) + pas::make_set<aGalaxyStruct::TWeaponAvailabilityMask>({{static_cast<std::int32_t>(aConst::OwnerWeaponAvailability[OwnerId])}});
        }
        if (pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Target->TypeId) && pas::contains(aConst::PlanetOwnerMasks.PirateClan, OwnerId)) {
            Available = Available + pas::make_set<aGalaxyStruct::TWeaponAvailabilityMask>({{static_cast<std::int32_t>(aConst::OwnerWeaponAvailability[OwnerId])}});
        }
        std::int32_t Attempts = 0;
        if (Attempts <= 100) {
            ++Attempts;
            Info = aGalaxy::Galaxy->SelectWeaponInfo(RandomState, Available, InventionLevels[7], 1);
            aMyFunction::AdvanceRandomSeed(RandomState);
            // The native comparison has no rejecting branch, but both counts are called.
            if (static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::stRanger, aGalaxyStruct::stPirate>(Target->TypeId) ^ 1) && pas::in_range(Info->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                static_cast<void>(Target->CountDirectFireWeapons() > Target->CountMissileWeapons());
            }
            MinSize = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[5]);
            MaxSize = System::Round(static_cast<long double>(Info->AverageSize) * aConst::EquipmentSizeFactors[1]);
            if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Target); warrior != nullptr && warrior->WarriorType == aWarrior::wtFlagship) {
                MinSize *= 2;
                MaxSize *= 2;
            }
            MinLevel = 1;
            MaxLevel = std::min<std::int32_t>(static_cast<std::int32_t>(InventionLevels[7]), static_cast<std::int32_t>(InventionLevels[Info->InventionIndex]));
            if (CurrentStar->Constellation->Id == 20) {
                MaxLevel = std::max<std::int32_t>(MaxLevel, static_cast<std::int32_t>(aGalaxy::Galaxy->TechLevel));
            }
            MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
            Owner = aConst::RaceToOwner(RaceId);
            if (OwnerId == aGalaxyStruct::oiPirate) {
                Owner = aGalaxyStruct::oiPirate;
            }
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, RandomState);
                std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, RandomState);
                Result = aItem::CreateGeneratedWeapon(Info, nextRandomIntRange_2, nextRandomIntRange, Owner);
            }
            if (aWarrior::TWarrior* warrior_2 = pas::class_cast_if<aWarrior::TWarrior*>(Target); warrior_2 != nullptr && warrior_2->WarriorType == aWarrior::wtFlagship) {
                Result->DetailImprovement = 3;
                Result->Improve(aItem::ikAny);
            } else if (Target->CanGenerateMicroModuleForLoadout()) {
                ModuleIndex = SelectWeaponOfferSpecialMicroModule(Result);
                if (ModuleIndex >= 0) {
                    aItem::ApplySpecialMicroModule(ModuleIndex, Result);
                }
            }
        }
        return Result;
    }

    aItem::TEquipment* TPlanet_GenerateEquipmentOffer(TPlanet* Self, void* Ship, aConst::TItemType ItemType) {
        std::int32_t Priority{};
        std::int32_t Attempts{};
        std::int32_t Module{};
        std::int32_t MinLevel{};
        std::int32_t MaxLevel{};
        std::int32_t MinSize{};
        std::int32_t MaxSize{};
        std::int32_t Special{};
        aGalaxyStruct::TOwnerId Owner{};
        aItem::TEquipment* Result = nullptr;
        if (Ship == nullptr || !(pas::class_cast_if<aShip::TShip*>(static_cast<pas::Object*>(Ship)) != nullptr)) {
            return Result;
        }
        aShip::TShip* Target = static_cast<aShip::TShip*>(Ship);
        if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            if (static_cast<std::uint8_t>(pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_FuelTanks), static_cast<std::int32_t>(aConst::t_Engine)) ^ 1) && Target->GetSlotCountForItemType(ItemType) == 0 && aPlayer::GetPlayer() != Target) {
                return Result;
            }
            MinLevel = 1;
            MaxLevel = Self->InventionLevels[aConst::EquipmentInventionIndices[ItemType]];
            if (Self->CurrentStar->Constellation->Id == 20) {
                MaxLevel = std::max<std::int32_t>(MaxLevel, static_cast<std::int32_t>(aGalaxy::Galaxy->TechLevel));
            }
            MinLevel = std::max<std::int32_t>(MinLevel, MaxLevel / 2 - 1);
            MinSize = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * aConst::EquipmentSizeFactors[5]);
            MaxSize = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * aConst::EquipmentSizeFactors[1]);
            if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Target); warrior != nullptr && warrior->WarriorType == aWarrior::wtFlagship) {
                MinSize *= 2;
                MaxSize *= 2;
            }
            Owner = aConst::RaceToOwner(Self->RaceId);
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                Owner = aGalaxyStruct::oiPirate;
            }
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(MinLevel, MaxLevel, Self->RandomState);
                std::int32_t nextRandomIntRange_2 = aMyFunction::NextRandomIntRange(MinSize, MaxSize, Self->RandomState);
                Result = aItem::CreateGeneratedEquipment(ItemType, nextRandomIntRange_2, nextRandomIntRange, Owner);
            }
            if (Target->CanGenerateMicroModuleForLoadout()) {
                Special = Self->SelectEquipmentOfferSpecialMicroModule(Result);
                if (Special >= 0) {
                    aItem::ApplySpecialMicroModule(Special, Result);
                }
            }
        } else if (pas::in_range(ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Result = Self->GenerateWeaponOffer(Ship);
        } else if (ItemType == aConst::t_Hull) {
            Result = Self->GenerateHullOffer(Ship);
        }
        if (Result != nullptr) {
            if (Result->CanImprove()) {
                std::int32_t cpp_case = aMyFunction::NextRandomIntRange(0, 100, Self->RandomState);
                if (cpp_case >= 0 && cpp_case <= 5) {
                    Result->Improve(aItem::ikMinor);
                } else if (cpp_case >= 6 && cpp_case <= 7) {
                    Result->Improve(aItem::ikMedium);
                }
            }
            Attempts = 0;
            if (Self->IsMainPiratePlanet && aMyFunction::NextRandomIntRange(0, 100, Self->RandomState) > 50) {
                do {
                    Priority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 7.0, 7.0E+1, 0.0));
                    {
                        std::uint32_t advanceRandomSeed = aMyFunction::AdvanceRandomSeed(Self->RandomState);
                        pas::Object* self = Self;
                        aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                        Module = aGalaxy::TGalaxy::SelectMicroModule(Priority + Attempts / 7, std::min<std::int32_t>(100, Priority + 10 + Attempts * 3), advanceRandomSeed, self);
                    }
                    if (aItem::CanInstallMicroModule(Module, Result)) {
                        aItem::ApplyMicroModule(Module, Result);
                        break;
                    }
                    ++Attempts;
                } while (!(Attempts > 50));
            }
        }
        return Result;
    }

    // Returns a new owning list of generated equipment, using the race quota table. Caller forwards ForceGeneratedOffers in CL; this routine saves but never reads it.
    aMyFunction::TObjectList* TPlanet_BuildEquipmentOfferBatch(TPlanet* Self, void* Ship, std::uint8_t UnusedForceGeneratedOffers) {
        std::int32_t i{};
        std::int32_t j{};
        aConst::TItemType ItemType{};
        aItem::TItem* Item{};
        aMyFunction::TObjectList* Offers = pas::make_object<aMyFunction::TObjectList>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, aConst::PlanetEquipmentOfferQuotas[Self->RaceId][0]); cpp_range.next(j); ) {
            Item = aPlanet::TPlanet_GenerateEquipmentOffer(Self, Ship, aConst::t_Hull);
            if (Item != nullptr) {
                pas::list_add(Offers, reinterpret_cast<void*>(Item));
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, aConst::CountItemTypesInMask(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}}))); cpp_range_2.next(i); ) {
            ItemType = static_cast<aConst::TItemType>(aConst::GetItemTypeFromMask(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}}), i));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, aConst::PlanetEquipmentOfferQuotas[Self->RaceId][ItemType - aConst::t_Hull]); cpp_range_3.next(j); ) {
                Item = aPlanet::TPlanet_GenerateEquipmentOffer(Self, Ship, ItemType);
                if (Item != nullptr) {
                    pas::list_add(Offers, reinterpret_cast<void*>(Item));
                }
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, aConst::PlanetEquipmentOfferQuotas[Self->RaceId][8]); cpp_range_4.next(i); ) {
            Item = aPlanet::TPlanet_GenerateEquipmentOffer(Self, Ship, aConst::t_Weapon1);
            if (Item != nullptr) {
                pas::list_add(Offers, reinterpret_cast<void*>(Item));
            }
        }
        return Offers;
    }

    // Population, economy and deterministic turn jitter adjust race quotas; clamps to 10..20.
    std::int32_t TPlanet::CalculateEquipmentShopTargetCount() {
        std::uint8_t ItemType{};
        std::int32_t Count = 0;
        for (ItemType = static_cast<std::uint8_t>(aConst::t_Hull); ItemType <= static_cast<std::uint8_t>(aConst::t_Weapon1); ++ItemType) {
            Count += aConst::PlanetEquipmentOfferQuotas[RaceId][ItemType - aConst::t_Hull];
        }
        std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(-2, 2, (GenerationSeed - aGalaxy::Galaxy->CurrentTurn) * 1011011);
        std::int32_t Result = System::Round(static_cast<long double>(aMyFunction::RemapClamped(Population, 1.0E+5, 1.0E+6, 0.5, 1.3)) * Count) + cpp_right;
        switch (Economy) {
            case aGalaxyStruct::peAgricultural: Result -= 2; break;
            case aGalaxyStruct::peIndustrial: Result += 2; break;
        }
        return std::max<std::int32_t>(10, std::min<std::int32_t>(Result, 20));
    }

    // Bucket 50 includes all weapon types 50..68; other buckets require an exact type.
    std::int32_t TPlanet::CountEquipmentShopItemsInBucket(aConst::TItemType ItemType) {
        std::int32_t i{};
        aItem::TItem* Item{};
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(i); ) {
            Item = pas::list_at<aItem::TItem>(EquipmentShop, i);
            if (Item->ItemType == ItemType || pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && ItemType == aConst::t_Weapon1) {
                ++Count;
            }
        }
        return Count;
    }

    // Frees at most one other stock item of the same type and level, protecting named script items. Does not insert Item.
    std::uint8_t TPlanet::RemoveSimilarEquipmentShopItem(aItem::TEquipment* Item) {
        std::int32_t i{};
        std::int32_t Index{};
        aItem::TEquipment* Candidate{};
        std::uint8_t Result = false;
        Index = aMyFunction::NextRandomIntRange(0, pas::list_count(EquipmentShop) - 1, RandomState);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(EquipmentShop) - 1); cpp_range.next(i); ) {
            aMyFunction::IncrementWrapped(Index, 0, pas::list_count(EquipmentShop) - 1);
            Candidate = pas::list_at<aItem::TEquipment>(EquipmentShop, Index);
            if (Item->ItemType == Candidate->ItemType && Candidate != Item && (Candidate->ScriptItem == nullptr || reinterpret_cast<aScript::TScriptItem*>(Candidate->ScriptItem)->Name == u"") && ([&] {
                std::int32_t cpp_left = Candidate->GetLevel();
                return cpp_left == Item->GetLevel();
            }())) {
                pas::list_delete(EquipmentShop, Index);
                pas::free(Candidate);
                return true;
            }
        }
        return Result;
    }

    void TPlanet::ForceGoodsScarcity(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask) {
        std::uint8_t Kind{};
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            if (pas::contains(GoodsMask, Kind)) {
                Goods[Kind].PriceState = aConst::GoodsMarket[Kind].MaxPrice;
                Goods[Kind].Count = std::min<std::int32_t>(Goods[Kind].Count, aConst::GoodsMarket[Kind].BaseStock / 10);
                Goods[Kind].PurchasePrice = System::Round(Goods[Kind].PriceState);
                Goods[Kind].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(Goods[Kind].PriceState * 0.98L - 1.0L));
                if (StartEvent) {
                    GoodsScarcityTicks[Kind] = System::Round(3.0E+1L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].GoodsEventDurationFactor);
                    GoodsSurplusTicks[Kind] = 0;
                } else if (GoodsScarcityTicks[Kind] > 0) {
                    --GoodsScarcityTicks[Kind];
                }
            }
        }
    }

    void TPlanet::ForceGoodsSurplus(std::uint8_t StartEvent, aGalaxyStruct::TItemTypeMask GoodsMask) {
        std::uint8_t Kind{};
        for (Kind = static_cast<std::uint8_t>(0); Kind <= static_cast<std::uint8_t>(7); ++Kind) {
            if (pas::contains(GoodsMask, Kind)) {
                Goods[Kind].PriceState = aConst::GoodsMarket[Kind].MinPrice;
                Goods[Kind].Count = std::max<std::int32_t>(Goods[Kind].Count, std::min<std::int32_t>(Goods[Kind].Count + aConst::GoodsMarket[Kind].BaseStock / 5, aConst::GoodsMarket[Kind].BaseStock * 3));
                Goods[Kind].PurchasePrice = System::Round(Goods[Kind].PriceState);
                Goods[Kind].BaseSalePrice = std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(Goods[Kind].PriceState * 0.98L - 1.0L));
                if (StartEvent) {
                    GoodsSurplusTicks[Kind] = System::Round(3.0E+1L * aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[1]].GoodsEventDurationFactor);
                    GoodsScarcityTicks[Kind] = 0;
                } else if (GoodsSurplusTicks[Kind] > 0) {
                    --GoodsSurplusTicks[Kind];
                }
            }
        }
    }

    // Counts local imprisoned rangers/pirates with a positive remaining prison term and no incompatible script state.
    std::int32_t TPlanet::CountBailablePrisoners() {
        std::int32_t i{};
        aShip::TShip* Ship{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range.next(i); ) {
            Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, i);
            if (Ship->CurrentPlanet == this && (Ship->ScriptShip == nullptr || pas::checked_cast<aScript::TScriptShip*>(Ship->ScriptShip)->State->StateKind == aScript::sskNormalAI) && aShip::TShip_IsInPrison(Ship) && Ship->GetPrisonTermRemaining() > 0) {
                ++Result;
            }
        }
        return Result;
    }

    pas::WideString TPlanet::GetGovernmentBackgroundGraph() {
        pas::WideString Result{};
        Result = u"GI,Bm."_w;
        if (IsMainPiratePlanet) {
            return pas::concat_wide({Result, u"Gov.PirateBG"});
        }
        Result = pas::concat_wide({Result, u"City.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName});
        if (OwnerId == aGalaxyStruct::oiPirate) {
            return pas::concat_wide({Result, u"Pirate"});
        }
        return Result;
    }

    pas::WideString TPlanet::BuildNonCivilTreasureHintText() {
        std::int32_t Index{};
        std::int32_t ItemLevel{};
        std::int32_t AverageSize{};
        aItem::TItem* Item{};
        aConst::PWeaponInfo WeaponInfo{};
        if (OwnerId != aGalaxyStruct::oiUninhabited || SurfaceLootEntries == nullptr || pas::list_count(SurfaceLootEntries) == 0) {
            return aConst::LocalizedText(u"Planet.NotCivil.Treasure.Nothing"_wref.get());
        }
        double Score = 0.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(SurfaceLootEntries) - 1); cpp_range.next(Index); ) {
            Item = pas::list_at<TPlanetSurfaceLootEntry>(SurfaceLootEntries, Index)->Item;
            if (pas::class_cast_if<aItem::TGoods*>(Item) != nullptr) {
                Score = Score + Item->Cost * aConst::GoodsMarket[Item->ItemType].AveragePrice * 1.0E-6L;
            } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                WeaponInfo = reinterpret_cast<aItem::TWeapon*>(Item)->GetWeaponInfo();
                ItemLevel = reinterpret_cast<aItem::TEquipment*>(Item)->GetLevel();
                AverageSize = aConst::GetAverageItemSize(Item->ItemType);
                Score = Score + pas::real_divide(Item->Cost * AverageSize, std::max<std::int32_t>(Item->Weight, 1)) * ItemLevel * WeaponInfo->TechLevel * 2.5E-5L;
            } else if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
                ItemLevel = reinterpret_cast<aItem::TEquipment*>(Item)->GetLevel();
                AverageSize = aConst::GetAverageItemSize(Item->ItemType);
                Score = Score + pas::real_divide(Item->Cost * AverageSize, std::max<std::int32_t>(Item->Weight, 1)) * pas::sqr(ItemLevel) * 2.5E-5L;
            } else if (aItem::TMicroModule* microModule = pas::class_cast_if<aItem::TMicroModule*>(Item)) {
                Score = Score + pas::real_divide(121.0L, aConst::MicroModuleTemplates[microModule->MicroModuleIndex - 1].Priority + 20);
            } else if (pas::class_cast_if<aItem::TArtefact*>(Item) != nullptr) {
                Score = Score + (pas::real_divide(aConst::GetAverageItemSize(Item->ItemType) * 0.5L, std::max<std::int32_t>(Item->Weight, 1)) + 0.5L) * 1.0E+1L;
            } else if (aItem::TEquipmentWithActCode* equipmentWithActCode = pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item); equipmentWithActCode != nullptr && equipmentWithActCode->DisplayAsArtefact) {
                Score = Score + 1.0E+1L;
            } else if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                Score = Score + 1.0L;
            } else {
                Score = Score + 0.001L;
            }
        }
        if (Score >= 15.0L) {
            return aConst::LocalizedText(u"Planet.NotCivil.Treasure.Lots"_wref.get());
        } else if (Score >= 5.0L) {
            return aConst::LocalizedText(u"Planet.NotCivil.Treasure.Many"_wref.get());
        } else if (Score >= 1.0L) {
            return aConst::LocalizedText(u"Planet.NotCivil.Treasure.Some"_wref.get());
        } else {
            return aConst::LocalizedText(u"Planet.NotCivil.Treasure.Few"_wref.get());
        }
    }

    pas::WideString TPlanet::BuildGovernmentGreeting() {
        pas::WideString Result{};
        pas::DynArray<Globals::TGovGreetingsInfo> Rules{};
        std::int32_t HighIndex{};
        Globals::TGovGreetingsInfo SwapA{};
        Globals::TGovGreetingsInfo SwapB{};
        pas::WideString UnusedText{};
        pas::WideString Greeting{};
        std::int32_t Attempt{};
        std::int32_t NearStarIndex{};
        std::int32_t I{};
        std::int32_t ShipCount{};
        std::int32_t RuleIndex{};
        std::uint8_t Good{};
        std::uint8_t Rejected{};
        std::uint8_t FoundPlanet{};
        aGalaxy::TStar* Star{};
        TPlanet* Planet{};
        std::uint8_t ShipType{};
        aGalaxyStruct::TGreetingCountMask CountMask{};
        aShip::TShip* Ship{};
        std::uint8_t CoalitionPresent{};
        std::uint8_t DominatorsPresent{};
        std::uint8_t PiratesPresent{};
        std::uint8_t PlayerPartyPresent{};
        std::uint8_t CustomPresent{};
        // Nested helper with caller-popped static link. Copies managed 0x44-byte greeting rules into ParentFrame-4; high index is at -8 and Self at -0C. Swaps each index 0..high div 2 with a seeded random index in 0..high; seed is Self.Id + 7*index + CurrentTurn div 7. Requires a nonempty rule table.
        auto PrepareRules = [&]() -> void {
            std::int32_t I{};
            std::int32_t J{};
            Rules.set_length(Globals::GovernmentGreetingCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, HighIndex); cpp_range.next(I); ) {
                Rules[I] = Globals::GovernmentGreetingDefinitions[I];
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, HighIndex / 2); cpp_range_2.next(I); ) {
                J = aMyFunction::SeededRandomIntRange(0, HighIndex, this->Id + 7 * I + aGalaxy::Galaxy->CurrentTurn / 7);
                SwapA = Rules[J];
                SwapB = Rules[I];
                Rules[I] = SwapA;
                Rules[J] = SwapB;
            }
        };
        std::int32_t BestPriority = -1;
        std::int32_t Priority = -1;
        CurrentStar->GetControlPresence(PlayerPartyPresent, CoalitionPresent, DominatorsPresent, PiratesPresent, CustomPresent);
        std::int32_t Minimum = 0;
        HighIndex = Globals::GovernmentGreetingCount - 1;
        PrepareRules();
        RuleIndex = aMyFunction::SeededRandomIntRange(0, HighIndex, static_cast<std::int32_t>(Id) * aGalaxy::Galaxy->CurrentTurn / 20);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, HighIndex); cpp_range.next(Attempt); ) {
            Greeting = pas::WideString();
            aMyFunction::IncrementWrapped(RuleIndex, Minimum, HighIndex);
            if (BestPriority > 0) {
                Priority = Rules[RuleIndex].Priority;
                {
                    std::int32_t cpp_left = Priority * aMyFunction::SeededRandomIntRange(1, 100, Id + RuleIndex * (aGalaxy::Galaxy->CurrentTurn / 20));
                    if (cpp_left < BestPriority * aMyFunction::SeededRandomIntRange(1, 100, Id + RuleIndex * (aGalaxy::Galaxy->CurrentTurn / 20) * 3)) {
                        continue;
                    }
                }
            }
            if (Rules[RuleIndex].PlayerRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].PlayerRace, aPlayer::GetPlayer()->PilotRace) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].PlayerStatus != pas::constant_set<aGalaxyStruct::TRangerCareerSet>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].PlayerStatus, aPlayer::GetPlayer()->GetDominantCareer()) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].PlayerRating != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].PlayerRating, aPlayer::GetPlayer()->GetRangerRatingBand()) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].PlayerRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].PlayerRank, aPlayer::GetPlayer()->Rank) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].PlayerPirateRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].PlayerPirateRank, aPlayer::GetPlayer()->PirateRank) ^ 1)) {
                continue;
            }
            Good = 50;
            if (Rules[RuleIndex].Goods != 42) {
                Good = Rules[RuleIndex].Goods;
            }
            Greeting = aConst::LocalizedColorText(pas::concat_wide({u"GovGreetings.", Rules[RuleIndex].Name, u".Text"}));
            if (Rules[RuleIndex].CurPlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].CurPlanetRace, RaceId) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].CurPlanetPirateClan != 2) {
                if (Rules[RuleIndex].CurPlanetPirateClan == 0 && OwnerId != aGalaxyStruct::oiPirate) {
                    continue;
                }
                if (Rules[RuleIndex].CurPlanetPirateClan == 1 && OwnerId == aGalaxyStruct::oiPirate) {
                    continue;
                }
            }
            if (Rules[RuleIndex].CurPlanetRaceIsPlayerRace != 2) {
                if (Rules[RuleIndex].CurPlanetRaceIsPlayerRace == 0 && aPlayer::GetPlayer()->PilotRace != RaceId) {
                    continue;
                }
                if (Rules[RuleIndex].CurPlanetRaceIsPlayerRace == 1 && aPlayer::GetPlayer()->PilotRace == RaceId) {
                    continue;
                }
            }
            if (Rules[RuleIndex].CurPlanetRelations != pas::constant_set<aGalaxyStruct::TRelationLevels>({}) && static_cast<std::uint8_t>(([&] {
                aGalaxyStruct::TRelationLevel cpp_element = GetRelationLevelToShip(aPlayer::GetPlayer());
                const aGalaxyStruct::TRelationLevels& cpp_set = Rules[RuleIndex].CurPlanetRelations;
                return pas::contains(cpp_set, cpp_element);
            }()) ^ 1)) {
                continue;
            }
            if (Good != 50) {
                if (Rules[RuleIndex].CurPlanetGoodsPermit != 2) {
                    if (Rules[RuleIndex].CurPlanetGoodsPermit == 0 && static_cast<std::uint8_t>(aConst::GoodsLegalOnPlanet[Good][RaceId][Government] ^ 1)) {
                        continue;
                    }
                    if (Rules[RuleIndex].CurPlanetGoodsPermit == 1 && aConst::GoodsLegalOnPlanet[Good][RaceId][Government] == true) {
                        continue;
                    }
                }
                if (Rules[RuleIndex].CurPlanetGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].CurPlanetGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(Goods[Good].Count, Good)) ^ 1)) {
                    continue;
                }
                if (Rules[RuleIndex].CurPlanetGoodsSale != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                    std::uint8_t cpp_element_2 = ([&] {
                        std::int32_t shopGoodsPurchasePrice = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, nullptr);
                        aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                        return galaxy->ClassifyGoodsPrice(shopGoodsPurchasePrice, Good);
                    }());
                    const Globals::TGreetingMask& cpp_set_2 = Rules[RuleIndex].CurPlanetGoodsSale;
                    return pas::contains(cpp_set_2, cpp_element_2);
                }()) ^ 1)) {
                    continue;
                }
                if (Rules[RuleIndex].CurPlanetGoodsBuy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                    std::uint8_t cpp_element_3 = ([&] {
                        std::int32_t shopGoodsSellPrice = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr);
                        aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                        return galaxy_2->ClassifyGoodsPrice(shopGoodsSellPrice, Good);
                    }());
                    const Globals::TGreetingMask& cpp_set_3 = Rules[RuleIndex].CurPlanetGoodsBuy;
                    return pas::contains(cpp_set_3, cpp_element_3);
                }()) ^ 1)) {
                    continue;
                }
            }
            if (Rules[RuleIndex].CurPlanetEconomy != pas::constant_set<aGalaxyStruct::TPlanetEconomies>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].CurPlanetEconomy, Economy) ^ 1)) {
                continue;
            }
            if (Rules[RuleIndex].CurPlanetGovernment != pas::constant_set<aGalaxyStruct::TPlanetGovernments>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].CurPlanetGovernment, Government) ^ 1)) {
                continue;
            }
            Rejected = false;
            for (ShipType = static_cast<std::uint8_t>(0); ShipType <= static_cast<std::uint8_t>(4); ++ShipType) {
                switch (ShipType) {
                    case 0: CountMask = Rules[RuleIndex].KlingInCurStar; break;
                    case 1: CountMask = Rules[RuleIndex].RangerInCurStar; break;
                    case 3: CountMask = Rules[RuleIndex].PirateInCurStar; break;
                    case 4: CountMask = Rules[RuleIndex].WarriorInCurStar; break;
                    case 2: CountMask = Rules[RuleIndex].TransportInCurStar; break;
                    default: GR_Main::RaiseWideMessage(u"function TPlanet.GovGreeting:WideString;"_wref.get()); break;
                }
                if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                    ShipCount = 0;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                        Ship = pas::list_at<aShip::TShip>(CurrentStar->Ships, I);
                        if (static_cast<std::uint8_t>(Ship->HasScriptStateText() ^ 1) && Ship->TypeNameOverrideKey == u"" && Ship->TypeId == ShipType) {
                            ++ShipCount;
                        }
                    }
                    ShipCount = std::min<std::int32_t>(10, ShipCount);
                    if (!pas::contains(CountMask, ShipCount)) {
                        Rejected = true;
                        break;
                    }
                }
            }
            if (Rejected) {
                continue;
            }
            if (Rules[RuleIndex].CurStarInBattle != 2) {
                if (Rules[RuleIndex].CurStarInBattle == 0 && !(static_cast<std::uint8_t>(CurrentStar->Status.Battle) && DominatorsPresent)) {
                    continue;
                }
                if (Rules[RuleIndex].CurStarInBattle == 1 && (static_cast<std::uint8_t>(CurrentStar->Status.Battle) && DominatorsPresent)) {
                    continue;
                }
            }
            if (Rules[RuleIndex].CurStarInBattlePirates != 2) {
                if (Rules[RuleIndex].CurStarInBattlePirates == 0 && !(static_cast<std::uint8_t>(CurrentStar->Status.Battle) && PiratesPresent)) {
                    continue;
                }
                if (Rules[RuleIndex].CurStarInBattlePirates == 1 && (static_cast<std::uint8_t>(CurrentStar->Status.Battle) && PiratesPresent)) {
                    continue;
                }
            }
            if (Rules[RuleIndex].CoalitionAlreadyDefeated != 2) {
                if (Rules[RuleIndex].CoalitionAlreadyDefeated == 0 && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    continue;
                }
                if (Rules[RuleIndex].CoalitionAlreadyDefeated == 1 && aGalaxy::Galaxy->CoalitionDefeatedTurn != 0) {
                    continue;
                }
            }
            if (Rules[RuleIndex].DominatorsAlreadyDefeated != 2) {
                if (Rules[RuleIndex].DominatorsAlreadyDefeated == 0 && aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}}))) {
                    continue;
                }
                if (Rules[RuleIndex].DominatorsAlreadyDefeated == 1 && static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1)) {
                    continue;
                }
            }
            if (Rules[RuleIndex].ToPlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                FoundPlanet = false;
                for (NearStarIndex = 0; NearStarIndex <= 7; ++NearStarIndex) {
                    Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(CurrentStar->StarDistances[NearStarIndex].Star));
                    if (!Star->IsConstellationVisible()) {
                        continue;
                    }
                    if (Star->Constellation->Id == 20) {
                        continue;
                    }
                    if (Star->Status.CustomFaction != u"") {
                        continue;
                    }
                    if (Rules[RuleIndex].ToPlanetInCurStar != 2) {
                        if (Rules[RuleIndex].ToPlanetInCurStar == 0 && CurrentStar != Star) {
                            continue;
                        }
                        if (Rules[RuleIndex].ToPlanetInCurStar == 1 && CurrentStar == Star) {
                            continue;
                        }
                    }
                    Rejected = false;
                    for (ShipType = static_cast<std::uint8_t>(0); ShipType <= static_cast<std::uint8_t>(4); ++ShipType) {
                        switch (ShipType) {
                            case 0: CountMask = Rules[RuleIndex].KlingInToStar; break;
                            case 1: CountMask = Rules[RuleIndex].RangerInToStar; break;
                            case 3: CountMask = Rules[RuleIndex].PirateInToStar; break;
                            case 4: CountMask = Rules[RuleIndex].WarriorInToStar; break;
                            case 2: CountMask = Rules[RuleIndex].TransportInToStar; break;
                            default: {
                                GR_Main::RaiseWideMessage(u"function TPlanet.GovGreeting:WideString;"_wref.get());
                                break;
                            }
                        }
                        if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                            ShipCount = 0;
                            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(I); ) {
                                Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                                if (static_cast<std::uint8_t>(Ship->HasScriptStateText() ^ 1) && Ship->TypeNameOverrideKey == u"" && Ship->TypeId == ShipType) {
                                    ++ShipCount;
                                }
                            }
                            ShipCount = std::min<std::int32_t>(10, ShipCount);
                            if (!pas::contains(CountMask, ShipCount)) {
                                Rejected = true;
                                break;
                            }
                        }
                    }
                    if (Rejected) {
                        continue;
                    }
                    if (Rules[RuleIndex].ToStarControlByKling != 2) {
                        if (Rules[RuleIndex].ToStarControlByKling == 0 && Star->Status.ControlFaction != aGalaxyStruct::sfDominators) {
                            continue;
                        }
                        if (Rules[RuleIndex].ToStarControlByKling == 1 && Star->Status.ControlFaction != aGalaxyStruct::sfCoalition) {
                            continue;
                        }
                    }
                    if (Rules[RuleIndex].ToStarInBattle != 2) {
                        if (Rules[RuleIndex].ToStarInBattle == 0 && static_cast<std::uint8_t>(static_cast<std::uint8_t>(Star->Status.Battle) ^ 1)) {
                            continue;
                        }
                        if (Rules[RuleIndex].ToStarInBattle == 1 && static_cast<std::uint8_t>(Star->Status.Battle)) {
                            continue;
                        }
                    }
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_4.next(I); ) {
                        Planet = pas::list_at<TPlanet>(Star->Planets, I);
                        if (Planet == this) {
                            continue;
                        }
                        if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                            continue;
                        }
                        if (!pas::contains(Rules[RuleIndex].ToPlanetRace, Planet->RaceId)) {
                            continue;
                        }
                        if (Rules[RuleIndex].ToPlanetRaceIsPlayerRace != 2) {
                            if (Rules[RuleIndex].ToPlanetRaceIsPlayerRace == 0 && aPlayer::GetPlayer()->PilotRace != Planet->RaceId) {
                                continue;
                            }
                            if (Rules[RuleIndex].ToPlanetRaceIsPlayerRace == 1 && aPlayer::GetPlayer()->PilotRace == Planet->RaceId) {
                                continue;
                            }
                        }
                        if (Rules[RuleIndex].ToPlanetRaceIsCurPlanetRace != 2) {
                            if (Rules[RuleIndex].ToPlanetRaceIsCurPlanetRace == 0 && RaceId != Planet->RaceId) {
                                continue;
                            }
                            if (Rules[RuleIndex].ToPlanetRaceIsCurPlanetRace == 1 && RaceId == Planet->RaceId) {
                                continue;
                            }
                        }
                        if (Rules[RuleIndex].ToPlanetRelations != pas::constant_set<aGalaxyStruct::TRelationLevels>({})) {
                            if (static_cast<std::uint8_t>(([&] {
                                aGalaxyStruct::TRelationLevel cpp_element_4 = Planet->GetRelationLevelToShip(aPlayer::GetPlayer());
                                const aGalaxyStruct::TRelationLevels& cpp_set_4 = Rules[RuleIndex].ToPlanetRelations;
                                return pas::contains(cpp_set_4, cpp_element_4);
                            }()) ^ 1) || Planet->OwnerId == aGalaxyStruct::oiPirate) {
                                continue;
                            }
                        }
                        if (Good != 50) {
                            if (Rules[RuleIndex].ToPlanetGoodsPermit != 2) {
                                if (Rules[RuleIndex].ToPlanetGoodsPermit == 0 && static_cast<std::uint8_t>(aConst::GoodsLegalOnPlanet[Good][Planet->RaceId][Planet->Government] ^ 1)) {
                                    continue;
                                }
                                if (Rules[RuleIndex].ToPlanetGoodsPermit == 1 && aConst::GoodsLegalOnPlanet[Good][Planet->RaceId][Planet->Government] == true) {
                                    continue;
                                }
                            }
                            if (Rules[RuleIndex].ToPlanetGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].ToPlanetGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(Planet->Goods[Good].Count, Good)) ^ 1)) {
                                continue;
                            }
                            if (Rules[RuleIndex].ToPlanetGoodsSale != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                                std::uint8_t cpp_element_5 = ([&] {
                                    std::int32_t shopGoodsPurchasePrice_2 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet);
                                    aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
                                    return galaxy_3->ClassifyGoodsPrice(shopGoodsPurchasePrice_2, Good);
                                }());
                                const Globals::TGreetingMask& cpp_set_5 = Rules[RuleIndex].ToPlanetGoodsSale;
                                return pas::contains(cpp_set_5, cpp_element_5);
                            }()) ^ 1)) {
                                continue;
                            }
                            if (Rules[RuleIndex].ToPlanetGoodsBuy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                                std::uint8_t cpp_element_6 = ([&] {
                                    std::int32_t shopGoodsSellPrice_2 = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet);
                                    aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                                    return galaxy_4->ClassifyGoodsPrice(shopGoodsSellPrice_2, Good);
                                }());
                                const Globals::TGreetingMask& cpp_set_6 = Rules[RuleIndex].ToPlanetGoodsBuy;
                                return pas::contains(cpp_set_6, cpp_element_6);
                            }()) ^ 1)) {
                                continue;
                            }
                        }
                        if (Rules[RuleIndex].ToPlanetEconomy != pas::constant_set<aGalaxyStruct::TPlanetEconomies>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].ToPlanetEconomy, Planet->Economy) ^ 1)) {
                            continue;
                        }
                        if (Rules[RuleIndex].ToPlanetGovernment != pas::constant_set<aGalaxyStruct::TPlanetGovernments>({}) && static_cast<std::uint8_t>(pas::contains(Rules[RuleIndex].ToPlanetGovernment, Planet->Government) ^ 1)) {
                            continue;
                        }
                        Greeting = aMyFunction::ReplaceColoredToken(Greeting, u"<ToPlanet>"_w, Planet->Name, u"<color=255,240,100>"_w);
                        Greeting = aMyFunction::ReplaceColoredToken(Greeting, u"<ToStar>"_w, Planet->CurrentStar->Name, u"<color=255,240,100>"_w);
                        if (Good != 50) {
                            Greeting = aMyFunction::ReplaceColoredToken(Greeting, u"<ToPlanetGoodsCnt>"_w, pas::wide_int_to_str(Planet->Goods[Good].Count), u"<color=255,240,100>"_w);
                            Greeting = aMyFunction::ReplaceColoredToken(Greeting, u"<ToPlanetGoodsSale>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet)), u"<color=255,240,100>"_w);
                            Greeting = aMyFunction::ReplaceColoredToken(Greeting, u"<ToPlanetGoodsBuy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet)), u"<color=255,240,100>"_w);
                        }
                        FoundPlanet = true;
                        break;
                    }
                    if (FoundPlanet) {
                        break;
                    }
                }
                if (!FoundPlanet) {
                    continue;
                }
            }
            if (Greeting != u"") {
                Result = Greeting;
                Result = aMyFunction::ReplaceColoredToken(Result, u"<PlayerRank>"_w, aPlayer::GetPlayer()->GetRankName(), u"<color=255,240,100>"_w);
                Result = aMyFunction::ReplaceColoredToken(Result, u"<CurPlanet>"_w, Name, u"<color=255,240,100>"_w);
                Result = aMyFunction::ReplaceColoredToken(Result, u"<CurStar>"_w, CurrentStar->Name, u"<color=255,240,100>"_w);
                if (Good != 50) {
                    Result = aMyFunction::ReplaceColoredToken(Result, u"<CurPlanetGoodsCnt>"_w, pas::wide_int_to_str(Goods[Good].Count), u"<color=255,240,100>"_w);
                    Result = aMyFunction::ReplaceColoredToken(Result, u"<CurPlanetGoodsSale>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, nullptr)), u"<color=255,240,100>"_w);
                    Result = aMyFunction::ReplaceColoredToken(Result, u"<CurPlanetGoodsBuy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr)), u"<color=255,240,100>"_w);
                }
                if (Priority == -1) {
                    BestPriority = Rules[RuleIndex].Priority;
                } else {
                    BestPriority = Priority;
                }
                if (BestPriority >= 50) {
                    break;
                }
            }
        }
        return Result;
    }

    void TSputnik::p_destroy() {
        aPlanet::TSputnik_Destroy(this);
    }

    void TPlanet::p_destroy() {
        aPlanet::TPlanet_Destroy(this);
    }

} // namespace aPlanet
