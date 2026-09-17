#include "layout/ab_Zone.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SystemImports.hpp"
#include "types/ab_MainForm.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_WorldImage.hpp"
#include "units/ab_WorldLine.hpp"
#include "units/ab_Zone.hpp"

// Native zone and link region:; original unit boundary unresolved.
namespace ab_Zone {
    void PropagateZoneDistances(PabZone Zone);

    std::uint32_t ZoneHeap = 0u;

    ab_Zone::PabZone FirstZone = nullptr;

    ab_Zone::PabZone LastZone = nullptr;

    ab_Zone::PabZone SelectedZone = nullptr;

    std::uint32_t ZoneLinkHeap = 0u;

    ab_Zone::PabZoneLink FirstZoneLink = nullptr;

    ab_Zone::PabZoneLink LastZoneLink = nullptr;

    ab_Zone::PabZoneLink SelectedZoneLink = nullptr;

    void ab_Zone_Clear() {
        while (!(FirstZone == nullptr)) {
            ab_Zone::ab_Zone_Delete(LastZone);
        }
        if (ZoneHeap != 0) {
            WindowsSdk::HeapDestroy(ZoneHeap);
            ZoneHeap = 0u;
        }
    }

    PabZone ab_Zone_Add() {
        if (ZoneHeap == 0) {
            ZoneHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (ZoneHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_Zone_Add.HeapCreate"_a));
            }
        }
        PabZone Entry = static_cast<PabZone>(EC_Mem::AllocClearFromHeapEC(ZoneHeap, static_cast<std::int32_t>(sizeof(TabZone))));
        if (LastZone != nullptr) {
            LastZone->Next = Entry;
        }
        Entry->Prev = LastZone;
        Entry->Next = nullptr;
        LastZone = Entry;
        if (FirstZone == nullptr) {
            FirstZone = Entry;
        }
        return Entry;
    }

    void ab_Zone_Delete(PabZone Zone) {
        std::int32_t Index{};
        PabZoneLink NextLink{};
        if (Zone->Prev != nullptr) {
            Zone->Prev->Next = Zone->Next;
        }
        if (Zone->Next != nullptr) {
            Zone->Next->Prev = Zone->Prev;
        }
        if (LastZone == Zone) {
            LastZone = Zone->Prev;
        }
        if (FirstZone == Zone) {
            FirstZone = Zone->Next;
        }
        PabZoneLink Link = FirstZoneLink;
        while (Link != nullptr) {
            NextLink = Link;
            Link = Link->Next;
            if (NextLink->First == Zone || NextLink->Last == Zone) {
                ab_Zone::ab_ZoneLink_Delete(NextLink);
            }
        }
        if (Zone->WorldLines != nullptr) {
            {
                const std::int32_t cpp_last = Zone->WorldLines.length() - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        if (Zone->WorldLines[Index] != nullptr) {
                            ab_WorldLine::ab_WorldLine_Delete(Zone->WorldLines[Index]);
                            Zone->WorldLines[Index] = nullptr;
                        }
                    }
                }
            }
            Zone->WorldLines = nullptr;
        }
        if (Zone->WorldImage != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Zone->WorldImage);
            Zone->WorldImage = nullptr;
        }
        ab_Zone::ab_Zone_ClearSegments(Zone);
        if (Zone->Routes != nullptr) {
            pas::free(Zone->Routes);
            Zone->Routes = nullptr;
        }
        if (SelectedZone == Zone) {
            SelectedZone = nullptr;
        }
        Zone->WorldImage = nullptr;
        Zone->Name = pas::WideString();
        if (ZoneHeap != 0) {
            EC_Mem::FreeFromHeapEC(ZoneHeap, Zone);
        }
    }

    void ab_Zone_UpdatePosition(PabZone Zone) {
        Zone->Radius = pas::real_divide(SystemImports::Pi * ab_Global::SphereRadius * Zone->RadiusDegrees, 1.8E+2L);
        pas::store_unaligned<EC_Struct::TVector3D>(&Zone->Position, ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Zone->Longitude), aMyFunction::HeadingDegreesToRadians(Zone->PolarAngle), ab_Global::SphereRadius));
    }

    void ab_Zone_UpdateImages(PabZone Zone) {
        std::int32_t Index{};
        std::uint32_t Color{};
        EC_Struct::TVector3D Last{};
        EC_Struct::TVector3D First{};
        ab_Global::TSphericalBearingState FirstState{};
        ab_Global::TSphericalBearingState LastState{};
        if (Zone->Kind == 1) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        } else if (Zone->Kind == 2) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
        } else if (Zone->Kind == 3) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
        } else if (Zone->Kind == 4) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 255);
        } else if (Zone->Kind == 5) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 155, 155);
        } else if (Zone->Kind == 6) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 255);
        } else if (Zone->Kind == 20) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 0);
        } else {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(200, 200, 0);
        }
        if (Zone->WorldLines == nullptr) {
            Zone->WorldLines.set_length(32);
            {
                const std::int32_t cpp_last = Zone->WorldLines.length() - 1;
                if (0 <= cpp_last) {
                    for (Index = 0; Index <= cpp_last; ++Index) {
                        Zone->WorldLines[Index] = nullptr;
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = Zone->WorldLines.length() - 1;
            if (0 <= cpp_last_2) {
                for (Index = 0; Index <= cpp_last_2; ++Index) {
                    if (Zone->WorldLines[Index] == nullptr) {
                        Zone->WorldLines[Index] = ab_WorldLine::ab_WorldLine_Create(EC_Struct::MakeVector3D(0.0, 0.0, 0.0), EC_Struct::MakeVector3D(0.0, 0.0, 0.0), 1, Color, 0u, false);
                    }
                }
            }
        }
        double Step = pas::real_divide(3.6E+2L, Zone->WorldLines.length() - 1 + 1 - 1);
        double Bearing = -Step;
        FirstState = ab_Global::AdvanceSphericalStateOnCurrentSphere(ab_Global::MakeSphericalBearingState(Zone->Longitude, Zone->PolarAngle, Bearing), Zone->Radius);
        First = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(FirstState.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(FirstState.PolarAngleDegrees), ab_Global::SphereRadius);
        {
            const std::int32_t cpp_last_3 = Zone->WorldLines.length() - 1;
            if (0 <= cpp_last_3) {
                for (Index = 0; Index <= cpp_last_3; ++Index) {
                    Bearing = static_cast<long double>(Bearing) + Step;
                    LastState = ab_Global::AdvanceSphericalStateOnCurrentSphere(ab_Global::MakeSphericalBearingState(Zone->Longitude, Zone->PolarAngle, Bearing), Zone->Radius);
                    Last = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(LastState.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(LastState.PolarAngleDegrees), ab_Global::SphereRadius);
                    ab_WorldLine::ab_WorldLine_Set(Zone->WorldLines[Index], First, Last, 1, Color, 0u, false);
                    First = Last;
                }
            }
        }
        if (Zone->Kind != 20) {
            if (Zone->WorldImage == nullptr) {
                Zone->WorldImage = ab_WorldImage::ab_WorldImage_Create(Zone->Position, u"GI,Bm.PI.Path4"_wref.get(), u""_wref.get(), false);
            } else {
                ab_WorldImage::ab_WorldImage_Set(Zone->WorldImage, Zone->Position, u"GI,Bm.PI.Path4"_wref.get(), u""_wref.get());
            }
        }
    }

    void ab_Zone_ClearImages() {
        std::int32_t Index{};
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->WorldLines != nullptr) {
                {
                    const std::int32_t cpp_last = Zone->WorldLines.length() - 1;
                    if (0 <= cpp_last) {
                        for (Index = 0; Index <= cpp_last; ++Index) {
                            if (Zone->WorldLines[Index] != nullptr) {
                                ab_WorldLine::ab_WorldLine_Delete(Zone->WorldLines[Index]);
                                Zone->WorldLines[Index] = nullptr;
                            }
                        }
                    }
                }
                Zone->WorldLines = nullptr;
            }
            if (Zone->WorldImage != nullptr) {
                ab_WorldImage::ab_WorldImage_Delete(Zone->WorldImage);
                Zone->WorldImage = nullptr;
            }
            Zone = Zone->Next;
        }
    }

    void ab_Zone_ClearSegments(PabZone Zone) {
        std::int32_t Index{};
        for (Index = 0; Index <= 3; ++Index) {
            if (pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Zone->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))) != nullptr) {
                Globals::ArcadeBattleScreen->WorldLines->RetireSegment(pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Zone->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))));
                pas::store_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Zone->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI)), nullptr);
            }
        }
    }

    std::int32_t ab_Zone_CountKind(std::int32_t Kind) {
        std::int32_t Result = 0;
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind == Kind) {
                ++Result;
            }
            Zone = Zone->Next;
        }
        return Result;
    }

    PabZone ab_Zone_Get(std::int32_t Index) {
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Index == 0) {
                return Zone;
            }
            --Index;
            Zone = Zone->Next;
        }
        return nullptr;
    }

    PabZone ab_Zone_GetKind(std::int32_t Kind, std::int32_t Index) {
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind == Kind) {
                if (Index == 0) {
                    return Zone;
                }
                --Index;
            }
            Zone = Zone->Next;
        }
        return nullptr;
    }

    void ab_ZoneLink_Clear() {
        while (!(FirstZoneLink == nullptr)) {
            ab_Zone::ab_ZoneLink_Delete(LastZoneLink);
        }
        if (ZoneLinkHeap != 0) {
            WindowsSdk::HeapDestroy(ZoneLinkHeap);
            ZoneLinkHeap = 0u;
        }
    }

    PabZoneLink ab_ZoneLink_Add() {
        if (ZoneLinkHeap == 0) {
            ZoneLinkHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (ZoneLinkHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_ZoneLink_Add.HeapCreate"_a));
            }
        }
        PabZoneLink Entry = static_cast<PabZoneLink>(EC_Mem::AllocClearFromHeapEC(ZoneLinkHeap, static_cast<std::int32_t>(sizeof(TabZoneLink))));
        if (LastZoneLink != nullptr) {
            LastZoneLink->Next = Entry;
        }
        Entry->Prev = LastZoneLink;
        Entry->Next = nullptr;
        LastZoneLink = Entry;
        if (FirstZoneLink == nullptr) {
            FirstZoneLink = Entry;
        }
        return Entry;
    }

    void ab_ZoneLink_Delete(PabZoneLink Link) {
        if (Link->Prev != nullptr) {
            Link->Prev->Next = Link->Next;
        }
        if (Link->Next != nullptr) {
            Link->Next->Prev = Link->Prev;
        }
        if (LastZoneLink == Link) {
            LastZoneLink = Link->Prev;
        }
        if (FirstZoneLink == Link) {
            FirstZoneLink = Link->Next;
        }
        if (Link->WorldLine != nullptr) {
            ab_WorldLine::ab_WorldLine_Delete(Link->WorldLine);
            Link->WorldLine = nullptr;
        }
        ab_Zone::ab_ZoneLink_ClearSegments(Link);
        if (SelectedZoneLink == Link) {
            SelectedZoneLink = nullptr;
        }
        if (ZoneLinkHeap != 0) {
            EC_Mem::FreeFromHeapEC(ZoneLinkHeap, Link);
        }
    }

    void ab_ZoneLink_UpdateDistance(PabZoneLink Link) {
        double Bearing{};
        ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing), pas::Var<double>(&Link->Distance), Link->First->Longitude, Link->First->PolarAngle, 0.0, Link->Last->Longitude, Link->Last->PolarAngle, ab_Global::SphereRadius);
    }

    void ab_ZoneLink_ClearImages() {
        PabZoneLink Link = FirstZoneLink;
        while (Link != nullptr) {
            if (Link->WorldLine != nullptr) {
                ab_WorldLine::ab_WorldLine_Delete(Link->WorldLine);
                Link->WorldLine = nullptr;
            }
            Link = Link->Next;
        }
    }

    void ab_ZoneLink_ClearSegments(PabZoneLink Link) {
        std::int32_t Index{};
        for (Index = 0; Index <= 1; ++Index) {
            if (pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Link->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))) != nullptr) {
                Globals::ArcadeBattleScreen->WorldLines->RetireSegment(pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Link->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))));
                pas::store_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Link->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI)), nullptr);
            }
        }
    }

    void ab_Zone_Load(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        PabZone Zone{};
        PabZoneLink Link{};
        ab_Zone::ab_ZoneLink_Clear();
        ab_Zone::ab_Zone_Clear();
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Zone = ab_Zone::ab_Zone_Add();
            Zone->Longitude = EC_Buf::TBufEC_GetSingle(Buffer);
            Zone->PolarAngle = EC_Buf::TBufEC_GetSingle(Buffer);
            Zone->RadiusDegrees = EC_Buf::TBufEC_GetSingle(Buffer);
            Zone->Kind = EC_Buf::TBufEC_GetInt32(Buffer);
            Zone->Name = Buffer->ReadWideString();
            Zone->BarrierHealth = EC_Buf::TBufEC_GetInt32(Buffer);
            Zone->GravityStrength = EC_Buf::TBufEC_GetInt32(Buffer);
            Zone->DamagePerTick = EC_Buf::TBufEC_GetInt32(Buffer);
            Zone->BonusFlags = EC_Buf::TBufEC_GetUInt32(Buffer);
            Zone->BonusRespawnClass = EC_Buf::TBufEC_GetInt32(Buffer);
            ab_Zone::ab_Zone_UpdatePosition(Zone);
        }
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Link = ab_Zone::ab_ZoneLink_Add();
            Link->First = ab_Zone::ab_Zone_Get(EC_Buf::TBufEC_GetInt32(Buffer));
            Link->Last = ab_Zone::ab_Zone_Get(EC_Buf::TBufEC_GetInt32(Buffer));
            Link->BarrierLinkMode = EC_Buf::TBufEC_GetInt32(Buffer);
            ab_Zone::ab_ZoneLink_UpdateDistance(Link);
        }
    }

    PabZone ab_Zone_RandomKind(std::int32_t Kind) {
        return ab_Zone::ab_Zone_GetKind(Kind, aMyFunction::RandomIntRange(0, ab_Zone::ab_Zone_CountKind(Kind) - 1));
    }

    ab_Global::TSphericalBearingState ab_Zone_RandomPosition(PabZone Zone) {
        ab_Global::TSphericalBearingState Result{};
        Result = ([&] {
            double cpp_arg = ([&] {
                pas::Extended cpp_left = pas::real_divide(([&] {
                    pas::Extended cpp_left_2 = pas::random_real(&System::RandSeed);
                    return cpp_left_2 * Zone->Radius;
                }()), 2.0L);
                return cpp_left + pas::real_divide(Zone->Radius, 4.0L);
            }());
            ab_Global::TSphericalBearingState makeSphericalBearingState = ([&] {
                double longitude = Zone->Longitude;
                double polarAngle = Zone->PolarAngle;
                double randomIntRange = aMyFunction::RandomIntRange(0, 360);
                return ab_Global::MakeSphericalBearingState(longitude, polarAngle, randomIntRange);
            }());
            return ab_Global::AdvanceSphericalStateOnCurrentSphere(makeSphericalBearingState, cpp_arg);
        }());
        return Result;
    }

    std::uint8_t ab_Zone_FindContainingOrNearest(double Longitude, double PolarAngle, PabZone& Nearest) {
        PabZone Zone{};
        double Distance{};
        double BestDistance{};
        std::uint8_t Result = false;
        Nearest = nullptr;
        if (FirstZone != nullptr) {
            BestDistance = 1.0E+20;
            Zone = FirstZone;
            while (Zone != nullptr) {
                if (Zone->Kind < 5) {
                    ab_Global::ComputeSphericalDistance(Distance, Zone->Longitude, Zone->PolarAngle, 0.0, Longitude, PolarAngle, ab_Global::SphereRadius);
                    if (Distance < Zone->Radius) {
                        Result = true;
                        Nearest = Zone;
                        return Result;
                    }
                    if (static_cast<long double>(Distance) - Zone->Radius < BestDistance) {
                        BestDistance = static_cast<long double>(Distance) - Zone->Radius;
                        Nearest = Zone;
                    }
                }
                Zone = Zone->Next;
            }
        }
        return Result;
    }

    PabZone ab_Zone_FindNearestOutside(double Longitude, double PolarAngle) {
        double Distance{};
        double BestDistance = 1.0E+20;
        PabZone Result = nullptr;
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind <= 1) {
                ab_Global::ComputeSphericalDistance(Distance, Zone->Longitude, Zone->PolarAngle, 0.0, Longitude, PolarAngle, ab_Global::SphereRadius);
                if (Distance > Zone->Radius && Distance < BestDistance) {
                    BestDistance = Distance;
                    Result = Zone;
                }
            }
            Zone = Zone->Next;
        }
        return Result;
    }

    PabZone ab_Zone_FindNearestEnabled(double Longitude, double PolarAngle) {
        PabZone Zone{};
        double Bearing{};
        double Distance{};
        double BestDistance{};
        PabZone Result = nullptr;
        if (FirstZone != nullptr) {
            BestDistance = 1.0E+20;
            Zone = FirstZone;
            while (Zone != nullptr) {
                if (Zone->GravityStrength != 0) {
                    ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing), pas::Var<double>(&Distance), Zone->Longitude, Zone->PolarAngle, 0.0, Longitude, PolarAngle, ab_Global::SphereRadius);
                    if (Distance < Zone->Radius) {
                        return Zone;
                    }
                    if (static_cast<long double>(Distance) - Zone->Radius < BestDistance) {
                        BestDistance = static_cast<long double>(Distance) - Zone->Radius;
                        Result = Zone;
                    }
                }
                Zone = Zone->Next;
            }
        }
        return Result;
    }

    std::uint8_t ab_Zone_IsInsideKind10(double Longitude, double PolarAngle) {
        double Distance{};
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind == 10) {
                ab_Global::ComputeSphericalDistance(Distance, Zone->Longitude, Zone->PolarAngle, 0.0, Longitude, PolarAngle, ab_Global::SphereRadius);
                if (Distance < Zone->Radius) {
                    return true;
                }
            }
            Zone = Zone->Next;
        }
        return false;
    }

    PabZone ab_Zone_FindRoute(PabZone Source, PabZone Target) {
        if (FirstZone == LastZone || FirstZoneLink == nullptr || Source == Target) {
            return nullptr;
        }
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            Zone->RouteDistance = 1.0E+20;
            Zone = Zone->Next;
        }
        Target->RouteDistance = 0.0;
        ab_Zone::PropagateZoneDistances(Target);
        PabZone Result = nullptr;
        double BestDistance = 1.0E+20;
        PabZoneLink Link = FirstZoneLink;
        while (Link != nullptr) {
            if (Link->First == Source) {
                if (Link->Last->RouteDistance < BestDistance) {
                    BestDistance = Link->Last->RouteDistance;
                    Result = Link->Last;
                }
            } else if (Link->Last == Source) {
                if (Link->First->RouteDistance < BestDistance) {
                    BestDistance = Link->First->RouteDistance;
                    Result = Link->First;
                }
            }
            Link = Link->Next;
        }
        return Result;
    }

    void ab_Zone_BuildRoutes(PabZone Zone) {
        if (Zone->Routes == nullptr) {
            Zone->Routes = pas::make_object<pas::List>();
        }
        pas::list_clear(Zone->Routes);
        PabZone Target = FirstZone;
        while (Target != nullptr) {
            if (Target->Kind < 5) {
                void* ab_Zone_FindRoute = static_cast<void*>(ab_Zone::ab_Zone_FindRoute(Zone, Target));
                pas::List* routes = Zone->Routes;
                pas::list_add(routes, ab_Zone_FindRoute);
            }
            Target = Target->Next;
        }
    }

    void ab_Zone_BuildAllRoutes() {
        std::int32_t Index = 0;
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind < 5) {
                Zone->RouteIndex = Index;
                ab_Zone::ab_Zone_BuildRoutes(Zone);
                ++Index;
            }
            Zone = Zone->Next;
        }
    }

    PabZone ab_Zone_GetRoute(PabZone Source, PabZone Target) {
        if (Target->Kind >= 5) {
            return nullptr;
        }
        return pas::list_at<TabZone>(Source->Routes, Target->RouteIndex);
    }

    std::uint8_t ab_Zone_IsHeadingInside(ab_Global::TSphericalBearingState Source, PabZone Zone, double& BearingDelta, double& AngularRadius) {
        std::uint8_t Result{};
        double Bearing{};
        double Distance{};
        ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&Bearing), pas::Var<double>(&Distance), Source.LongitudeDegrees, Source.PolarAngleDegrees, Source.BearingDegrees, Zone->Longitude, Zone->PolarAngle, ab_Global::SphereRadius);
        if (Zone->Radius >= Distance) {
            Result = true;
            BearingDelta = 0.0;
        } else {
            AngularRadius = aMyFunction::RadiansToHeadingDegrees(MathImports::ArcSin(pas::real_divide(Zone->Radius, Distance)));
            Result = std::fabs(static_cast<pas::Extended>(Bearing)) < AngularRadius;
            BearingDelta = Bearing;
        }
        return Result;
    }

    PabZone ab_Zone_FindReachableRouteZone(PabZone Source) {
        double Distance{};
        if (Source->Kind < 5) {
            return Source;
        }
        PabZone Result = nullptr;
        double BestDistance = 1.0E+20;
        PabZone Zone = FirstZone;
        while (Zone != nullptr) {
            if (Zone != Source && Zone->Kind < 5) {
                ab_Global::ComputeSphericalDistance(Distance, Source->Longitude, Source->PolarAngle, 0.0, Zone->Longitude, Zone->PolarAngle, ab_Global::SphereRadius);
                if (Distance < BestDistance && static_cast<std::uint8_t>(ab_StopLine::ab_StopLine_IsBlocked(Source->Longitude, Source->PolarAngle, Zone->Longitude, Zone->PolarAngle) ^ 1)) {
                    Result = Zone;
                    BestDistance = Distance;
                }
            }
            Zone = Zone->Next;
        }
        return Result;
    }

    PabZone ab_Zone_RandomRoute(PabZone Source, std::int32_t Steps) {
        PabZone Candidate{};
        std::int32_t Attempts{};
        PabZone Result = nullptr;
        if (Source->Routes == nullptr || pas::list_count(Source->Routes) < 1) {
            return Result;
        }
        PabZone Current = Source;
        while (Steps > 0) {
            --Steps;
            Attempts = 5;
            while (Attempts > 0) {
                --Attempts;
                Candidate = ([&] {
                    std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, pas::list_count(Current->Routes) - 1);
                    pas::List* routes = Current->Routes;
                    return pas::list_at<TabZone>(routes, randomIntRange);
                }());
                if (Candidate != nullptr && Candidate->Routes != nullptr && pas::list_count(Candidate->Routes) >= 1 && Candidate != Source && Candidate != Current) {
                    Current = Candidate;
                    break;
                }
            }
            if (Attempts <= 0) {
                return Result;
            }
        }
        return Current;
    }

    void PropagateZoneDistances(PabZone Zone) {
        PabZoneLink Link = FirstZoneLink;
        while (Link != nullptr) {
            if (Link->First == Zone) {
                if (static_cast<long double>(Zone->RouteDistance) + Link->Distance < Link->Last->RouteDistance) {
                    Link->Last->RouteDistance = static_cast<long double>(Zone->RouteDistance) + Link->Distance;
                    ab_Zone::PropagateZoneDistances(Link->Last);
                }
            } else if (Link->Last == Zone) {
                if (static_cast<long double>(Zone->RouteDistance) + Link->Distance < Link->First->RouteDistance) {
                    Link->First->RouteDistance = static_cast<long double>(Zone->RouteDistance) + Link->Distance;
                    ab_Zone::PropagateZoneDistances(Link->First);
                }
            }
            Link = Link->Next;
        }
    }

} // namespace ab_Zone
