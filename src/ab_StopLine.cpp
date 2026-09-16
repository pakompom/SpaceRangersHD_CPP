#include "layout/ab_StopLine.hpp"
#include "types/GI_PolyLine.hpp"
#include "types/SystemImports.hpp"
#include "types/ab_MainForm.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/Globals.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_WorldImage.hpp"
#include "units/ab_WorldLine.hpp"

// Native stop-point/line region:, including compiler-managed finalization.
namespace ab_StopLine {
    std::uint32_t StopPointHeap = 0u;

    ab_StopLine::PabStopPoint FirstStopPoint = nullptr;

    ab_StopLine::PabStopPoint LastStopPoint = nullptr;

    ab_StopLine::PabStopPoint SelectedStopPoint = nullptr;

    std::uint32_t StopLineHeap = 0u;

    ab_StopLine::PabStopLine FirstStopLine = nullptr;

    ab_StopLine::PabStopLine LastStopLine = nullptr;

    ab_StopLine::PabStopLine SelectedStopLine = nullptr;

    ab_StopLine::PabStopLine FirstCollisionLine = nullptr;

    pas::DynArray<ab_StopLine::PabStopPoint> StopPointIndex{};

    void ab_StopPoint_Clear() {
        ab_StopLine::ab_StopPoint_ClearIndex();
        while (!(FirstStopPoint == nullptr)) {
            ab_StopLine::ab_StopPoint_Delete(LastStopPoint);
        }
        if (StopPointHeap != 0) {
            WindowsSdk::HeapDestroy(StopPointHeap);
            StopPointHeap = 0u;
        }
    }

    // Allocates and links a node owned by the world list.
    PabStopPoint ab_StopPoint_Add() {
        if (StopPointHeap == 0) {
            StopPointHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (StopPointHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_StopPoint_Add.HeapCreate"_a));
            }
        }
        PabStopPoint Entry = static_cast<PabStopPoint>(EC_Mem::AllocClearFromHeapEC(StopPointHeap, static_cast<std::int32_t>(sizeof(TabStopPoint))));
        Entry->Radius = ab_Global::SphereRadius;
        if (LastStopPoint != nullptr) {
            LastStopPoint->Next = Entry;
        }
        Entry->Prev = LastStopPoint;
        Entry->Next = nullptr;
        LastStopPoint = Entry;
        if (FirstStopPoint == nullptr) {
            FirstStopPoint = Entry;
        }
        return Entry;
    }

    void ab_StopPoint_Delete(PabStopPoint Point) {
        PabStopLine NextLine{};
        if (Point->Prev != nullptr) {
            Point->Prev->Next = Point->Next;
        }
        if (Point->Next != nullptr) {
            Point->Next->Prev = Point->Prev;
        }
        if (LastStopPoint == Point) {
            LastStopPoint = Point->Prev;
        }
        if (FirstStopPoint == Point) {
            FirstStopPoint = Point->Next;
        }
        PabStopLine Line = FirstStopLine;
        while (Line != nullptr) {
            NextLine = Line;
            Line = Line->Next;
            if (NextLine->First == Point || NextLine->Last == Point) {
                ab_StopLine::ab_StopLine_Delete(NextLine);
            }
        }
        if (Point->WorldImage != nullptr) {
            ab_WorldImage::ab_WorldImage_Delete(Point->WorldImage);
            Point->WorldImage = nullptr;
        }
        ab_StopLine::ab_StopPoint_ClearSegments(Point);
        if (SelectedStopPoint == Point) {
            SelectedStopPoint = nullptr;
        }
        if (StopPointHeap != 0) {
            EC_Mem::FreeFromHeapEC(StopPointHeap, Point);
        }
    }

    // Nil updates every point.
    void ab_StopPoint_UpdatePosition(PabStopPoint Point) {
        if (Point == nullptr) {
            Point = FirstStopPoint;
            while (Point != nullptr) {
                pas::store_unaligned<EC_Struct::TVector3D>(&Point->Position, ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Point->Longitude), aMyFunction::HeadingDegreesToRadians(Point->PolarAngle), Point->Radius));
                Point = Point->Next;
            }
        } else {
            pas::store_unaligned<EC_Struct::TVector3D>(&Point->Position, ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Point->Longitude), aMyFunction::HeadingDegreesToRadians(Point->PolarAngle), Point->Radius));
        }
    }

    void ab_StopPoint_ClearImages() {
        PabStopPoint Point = FirstStopPoint;
        while (Point != nullptr) {
            if (Point->WorldImage != nullptr) {
                ab_WorldImage::ab_WorldImage_Delete(Point->WorldImage);
                Point->WorldImage = nullptr;
            }
            Point = Point->Next;
        }
    }

    void ab_StopPoint_ClearSegments(PabStopPoint Point) {
        std::int32_t Index{};
        for (Index = 0; Index <= 3; ++Index) {
            if (pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Point->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))) != nullptr) {
                Globals::ArcadeBattleScreen->WorldLines->RetireSegment(pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Point->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))));
                pas::store_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Point->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI)), nullptr);
            }
        }
    }

    void ab_StopPoint_BuildIndex() {
        ab_StopLine::ab_StopPoint_ClearIndex();
        std::int32_t Count = ab_StopLine::ab_StopPoint_Count();
        StopPointIndex.set_length(Count);
        std::int32_t Index = 0;
        PabStopPoint Point = FirstStopPoint;
        while (Point != nullptr) {
            StopPointIndex[Index] = Point;
            ++Index;
            Point = Point->Next;
        }
    }

    void ab_StopPoint_ClearIndex() {
        StopPointIndex = nullptr;
    }

    std::int32_t ab_StopPoint_Count() {
        std::int32_t Result = 0;
        PabStopPoint Point = FirstStopPoint;
        while (Point != nullptr) {
            ++Result;
            Point = Point->Next;
        }
        return Result;
    }

    void ab_StopLine_Clear() {
        while (!(FirstStopLine == nullptr)) {
            ab_StopLine::ab_StopLine_Delete(LastStopLine);
        }
        if (StopLineHeap != 0) {
            WindowsSdk::HeapDestroy(StopLineHeap);
            StopLineHeap = 0u;
        }
    }

    // Allocates and links a node owned by the world list.
    PabStopLine ab_StopLine_Add() {
        if (StopLineHeap == 0) {
            StopLineHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (StopLineHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_StopLine_Add.HeapCreate"_a));
            }
        }
        PabStopLine Entry = static_cast<PabStopLine>(EC_Mem::AllocClearFromHeapEC(StopLineHeap, static_cast<std::int32_t>(sizeof(TabStopLine))));
        if (LastStopLine != nullptr) {
            LastStopLine->Next = Entry;
        }
        Entry->Prev = LastStopLine;
        Entry->Next = nullptr;
        LastStopLine = Entry;
        if (FirstStopLine == nullptr) {
            FirstStopLine = Entry;
        }
        Entry->Collidable = true;
        Entry->Visible = true;
        Entry->UserValue = 0;
        return Entry;
    }

    void ab_StopLine_Delete(PabStopLine Line) {
        if (Line->Prev != nullptr) {
            Line->Prev->Next = Line->Next;
        }
        if (Line->Next != nullptr) {
            Line->Next->Prev = Line->Prev;
        }
        if (LastStopLine == Line) {
            LastStopLine = Line->Prev;
        }
        if (FirstStopLine == Line) {
            FirstStopLine = Line->Next;
        }
        if (Line->WorldLine != nullptr) {
            ab_WorldLine::ab_WorldLine_Delete(Line->WorldLine);
            Line->WorldLine = nullptr;
        }
        ab_StopLine::ab_StopLine_ClearSegments(Line);
        if (SelectedStopLine == Line) {
            SelectedStopLine = nullptr;
        }
        if (StopLineHeap != 0) {
            EC_Mem::FreeFromHeapEC(StopLineHeap, Line);
        }
    }

    void ab_StopLine_AddLatitude(double PolarAngle, double Step) {
        PabStopLine Line{};
        PabStopPoint Point{};
        PabStopPoint First = ab_StopLine::ab_StopPoint_Add();
        First->Longitude = 0.0;
        First->PolarAngle = PolarAngle;
        First->Kind = 1;
        ab_StopLine::ab_StopPoint_UpdatePosition(First);
        PabStopPoint Previous = First;
        double Longitude = Step;
        while (Longitude < 3.6E+2L) {
            Point = ab_StopLine::ab_StopPoint_Add();
            Point->Longitude = Longitude;
            Point->PolarAngle = PolarAngle;
            Point->Kind = 1;
            ab_StopLine::ab_StopPoint_UpdatePosition(Point);
            Line = ab_StopLine::ab_StopLine_Add();
            Line->First = Previous;
            Line->Last = Point;
            Previous = Point;
            Longitude = static_cast<long double>(Longitude) + Step;
        }
        Line = ab_StopLine::ab_StopLine_Add();
        Line->First = Previous;
        Line->Last = First;
    }

    void ab_StopLine_UpdateWorldLines() {
        PabStopLine Line = FirstStopLine;
        while (Line != nullptr) {
            if (Line->Visible) {
                if (Line->WorldLine == nullptr) {
                    Line->WorldLine = ab_WorldLine::ab_WorldLine_Create(Line->First->Position, Line->Last->Position, 4, pas::load_unaligned<std::uint32_t>(Line->FirstColor), 0x80ffffffu, false);
                    Line->WorldLine->FrontEndColor = pas::load_unaligned<std::uint32_t>(Line->LastColor);
                    Line->WorldLine->BackEndColor = 0x80ffffffu;
                } else {
                    ab_WorldLine::ab_WorldLine_Set(Line->WorldLine, Line->First->Position, Line->Last->Position, 4, pas::load_unaligned<std::uint32_t>(Line->FirstColor), 0x80ffffffu, false);
                    Line->WorldLine->FrontEndColor = pas::load_unaligned<std::uint32_t>(Line->LastColor);
                    Line->WorldLine->BackEndColor = 0x80ffffffu;
                }
            }
            Line = Line->Next;
        }
    }

    void ab_StopLine_UpdateColors() {
        PabStopLine Line = FirstStopLine;
        while (Line != nullptr) {
            if (Line->WorldLine != nullptr) {
                Line->WorldLine->FrontColor = pas::load_unaligned<std::uint32_t>(Line->FirstColor);
                Line->WorldLine->FrontEndColor = pas::load_unaligned<std::uint32_t>(Line->LastColor);
            }
            Line = Line->Next;
        }
    }

    void ab_StopLine_ClearSegments(PabStopLine Line) {
        std::int32_t Index{};
        for (Index = 0; Index <= 1; ++Index) {
            if (pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Line->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))) != nullptr) {
                Globals::ArcadeBattleScreen->WorldLines->RetireSegment(pas::load_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Line->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI))));
                pas::store_unaligned<GI_PolyLine::PPolyLineSegmentGI>(pas::byte_offset(&Line->Segments, Index * sizeof(GI_PolyLine::PPolyLineSegmentGI)), nullptr);
            }
        }
    }

    // Empty in this native version.
    void ab_StopLine_PrepareCollision(PabStopLine Line) {
    }

    void ab_StopLine_BuildCollisionList() {
        PabStopLine Previous = nullptr;
        FirstCollisionLine = nullptr;
        PabStopLine Line = FirstStopLine;
        while (Line != nullptr) {
            ab_StopLine::ab_StopLine_PrepareCollision(Line);
            Line->NextCollision = nullptr;
            if (Line->Collidable) {
                if (Previous == nullptr) {
                    FirstCollisionLine = Line;
                } else {
                    Previous->NextCollision = Line;
                }
                Previous = Line;
            }
            Line = Line->Next;
        }
    }

    std::uint8_t ab_StopLine_ReflectMovement(ab_Global::TSphericalBearingState Source, ab_Global::TSphericalBearingState Target, double& HeadingDelta, double& Speed, double& UnusedResult) {
        float InverseLengthSquared{};
        float LineLength{};
        PabStopLine Line{};
        double Factor{};
        double CenterDepth{};
        double FirstT{};
        double SecondT{};
        double OriginalHeading{};
        double Distance{};
        float LengthSquared{};
        ab_Global::TMatrix4D Matrix{};
        EC_Struct::TVector3D Normal{};
        EC_Struct::TVector3D A{};
        EC_Struct::TVector3D B{};
        EC_Struct::TVector3D Direction{};
        EC_Struct::TVector3D Movement{};
        auto IntersectCollisionParameters = [&](EC_Struct::TVector3D A, EC_Struct::TVector3D B, EC_Struct::TVector3D C, EC_Struct::TVector3D D, double& FirstT, double& SecondT) -> std::uint8_t {
            FirstT = (static_cast<long double>(B.X) - A.X) * (static_cast<long double>(D.Y) - C.Y) - (static_cast<long double>(B.Y) - A.Y) * (static_cast<long double>(D.X) - C.X);
            if (FirstT == 0.0L) {
                return false;
            }
            FirstT = pas::real_divide(1.0L, FirstT);
            SecondT = ((static_cast<long double>(A.Y) - C.Y) * (static_cast<long double>(B.X) - A.X) - (static_cast<long double>(A.X) - C.X) * (static_cast<long double>(B.Y) - A.Y)) * FirstT;
            FirstT = ((static_cast<long double>(A.Y) - C.Y) * (static_cast<long double>(D.X) - C.X) - (static_cast<long double>(A.X) - C.X) * (static_cast<long double>(D.Y) - C.Y)) * FirstT;
            return true;
        };
        auto CollisionLineDistance = [&](EC_Struct::TVector3D A, EC_Struct::TVector3D B, EC_Struct::TVector3D Point) -> double {
            double Cross = (static_cast<long double>(A.Y) - Point.Y) * (static_cast<long double>(B.X) - A.X) - (static_cast<long double>(A.X) - Point.X) * (static_cast<long double>(B.Y) - A.Y);
            return static_cast<long double>(Cross) * InverseLengthSquared * LineLength;
        };
        std::uint8_t Result = false;
        if (FirstStopLine != nullptr) {
            UnusedResult = 0.0;
            A = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Source.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(Source.PolarAngleDegrees), ab_Global::SphereCameraDistance);
            B = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
            Direction = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Source.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(Source.PolarAngleDegrees + 9.0E+1L)), ab_Global::SphereCameraDistance);
            Matrix = ab_Global::BuildLookAtMatrix(A, B, Direction);
            Movement = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Target.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(Target.PolarAngleDegrees), ab_Global::SphereRadius);
            Movement = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Movement));
            OriginalHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Movement.X, -Movement.Y));
            A = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
            A = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&A));
            CenterDepth = A.Z;
            Line = FirstCollisionLine;
            while (Line != nullptr) {
                A = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->First->Position));
                B = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->Last->Position));
                if (!(A.Z < CenterDepth && B.Z < CenterDepth)) {
                    Line = Line->NextCollision;
                    continue;
                }
                Direction.X = static_cast<long double>(B.X) - A.X;
                Direction.Y = static_cast<long double>(B.Y) - A.Y;
                LengthSquared = pas::sqr(static_cast<pas::Extended>(Direction.X)) + pas::sqr(static_cast<pas::Extended>(Direction.Y));
                LineLength = System::Sqrt(LengthSquared);
                InverseLengthSquared = pas::real_divide(1.0L, LengthSquared);
                Distance = std::fabs(static_cast<pas::Extended>(CollisionLineDistance(A, B, EC_Struct::MakeVector3D(0.0, 0.0, 0.0))));
                if (!(Distance <= 11.0L)) {
                    Line = Line->NextCollision;
                    continue;
                }
                Factor = pas::real_divide(1.0L, LineLength);
                Direction.X = static_cast<long double>(Direction.X) * Factor;
                Direction.Y = static_cast<long double>(Direction.Y) * Factor;
                if (!IntersectCollisionParameters(A, B, EC_Struct::MakeVector3D(0.0, 0.0, 0.0), Movement, FirstT, SecondT)) {
                    Line = Line->NextCollision;
                    continue;
                }
                if (!(FirstT >= -1.0E-4L && FirstT <= 1.0001L)) {
                    Line = Line->NextCollision;
                    continue;
                }
                if (!(SecondT >= 0.0L)) {
                    Line = Line->NextCollision;
                    continue;
                }
                Normal.X = -Direction.Y;
                Normal.Y = Direction.X;
                Factor = static_cast<long double>(-Movement.X) * Normal.X + static_cast<long double>(-Movement.Y) * Normal.Y;
                Movement.X = (static_cast<long double>(Normal.X) * Factor * 2.0L + Movement.X) * 0.8L;
                Movement.Y = (static_cast<long double>(Normal.Y) * Factor * 2.0L + Movement.Y) * 0.8L;
                Result = true;
                break;
            }
            if (Result) {
                HeadingDelta = aMyFunction::HeadingDifferenceDegrees(OriginalHeading, aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(Movement.X, -Movement.Y)));
                Speed = System::Sqrt(pas::sqr(static_cast<pas::Extended>(Movement.X)) + pas::sqr(static_cast<pas::Extended>(Movement.Y)));
            }
        }
        return Result;
    }

    void ab_StopLine_GetDistances(ab_Global::TSphericalBearingState Source, double& ForwardDistance, double& BackwardDistance) {
        double CenterDepth{};
        PabStopLine Line{};
        ab_Global::TMatrix4D Matrix{};
        ab_Global::TMatrix4D View{};
        ab_Global::TMatrix4D Rotation{};
        EC_Struct::TVector3D A{};
        EC_Struct::TVector3D B{};
        EC_Struct::TVector3D Hit{};
        auto IntersectCollisionLines = [&](EC_Struct::TVector3D A, EC_Struct::TVector3D B, EC_Struct::TVector3D C, EC_Struct::TVector3D D, EC_Struct::TVector3D& Hit) -> std::uint8_t {
            double DX1 = static_cast<long double>(B.X) - A.X;
            double DY1 = static_cast<long double>(B.Y) - A.Y;
            double DX2 = static_cast<long double>(D.X) - C.X;
            double DY2 = static_cast<long double>(D.Y) - C.Y;
            double Denominator = static_cast<long double>(DY1) * DX2 - static_cast<long double>(DY2) * DX1;
            if (Denominator == 0.0L) {
                return false;
            }
            Hit.X = pas::real_divide((static_cast<long double>(C.Y) - A.Y) * DX1 * DX2 + static_cast<long double>(DY1) * DX2 * A.X - static_cast<long double>(DY2) * DX1 * C.X, Denominator);
            if (DX1 != 0.0L) {
                Hit.Y = pas::real_divide((static_cast<long double>(Hit.X) - A.X) * DY1, DX1) + A.Y;
            } else {
                Hit.Y = pas::real_divide((static_cast<long double>(Hit.X) - C.X) * DY2, DX2) + C.Y;
            }
            return true;
        };
        ForwardDistance = 1.0E+20;
        BackwardDistance = 1.0E+20;
        if (FirstStopLine != nullptr) {
            A = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Source.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(Source.PolarAngleDegrees), ab_Global::SphereCameraDistance);
            B = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
            Hit = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(Source.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(Source.PolarAngleDegrees + 9.0E+1L), ab_Global::SphereCameraDistance);
            View = ab_Global::BuildLookAtMatrix(A, B, Hit);
            Rotation = ab_Global::BuildZAxisRotationMatrix(aMyFunction::HeadingDegreesToRadians(Source.BearingDegrees));
            Matrix = ab_Global::MultiplyMatrix4D(Rotation, View);
            A = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
            A = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&A));
            CenterDepth = A.Z;
            Line = FirstCollisionLine;
            while (Line != nullptr) {
                A = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->First->Position));
                B = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Line->Last->Position));
                if (!(A.Z < CenterDepth && B.Z < CenterDepth)) {
                    Line = Line->NextCollision;
                    continue;
                }
                if (!((A.X >= 0.0L || B.X >= 0.0L) && (A.X <= 0.0L || B.X <= 0.0L))) {
                    Line = Line->NextCollision;
                    continue;
                }
                if (!IntersectCollisionLines(A, B, EC_Struct::MakeVector3D(0.0, 0.0, 0.0), EC_Struct::MakeVector3D(0.0, 1.0, 0.0), Hit)) {
                    Line = Line->NextCollision;
                    continue;
                }
                if (Hit.Y <= 0.0L && -Hit.Y < ForwardDistance) {
                    ForwardDistance = -Hit.Y;
                }
                if (Hit.Y >= 0.0L && BackwardDistance > Hit.Y) {
                    BackwardDistance = Hit.Y;
                }
                Line = Line->NextCollision;
            }
            if (ForwardDistance < 1.0E+15L && ForwardDistance != 0.0L) {
                pas::Extended cpp_left = aMyFunction::RadiansToHeadingDegrees(MathImports::ArcSin(pas::real_divide(ForwardDistance, ab_Global::SphereRadius)));
                ForwardDistance = cpp_left * pas::real_divide(SystemImports::Pi * ab_Global::SphereRadius, 1.8E+2L);
            }
            if (BackwardDistance < 1.0E+15L && BackwardDistance != 0.0L) {
                pas::Extended cpp_left_2 = aMyFunction::RadiansToHeadingDegrees(MathImports::ArcSin(pas::real_divide(BackwardDistance, ab_Global::SphereRadius)));
                BackwardDistance = cpp_left_2 * pas::real_divide(SystemImports::Pi * ab_Global::SphereRadius, 1.8E+2L);
            }
        }
    }

    std::uint8_t ab_StopLine_IsBlocked(double SourceLongitude, double SourcePolarAngle, double TargetLongitude, double TargetPolarAngle) {
        double BearingDelta{};
        double Distance{};
        double ForwardDistance{};
        double BackwardDistance{};
        ab_Global::ComputeSphericalBearingAndDistance(BearingDelta, pas::Var<double>(&Distance), SourceLongitude, SourcePolarAngle, 0.0, TargetLongitude, TargetPolarAngle, ab_Global::SphereRadius);
        ab_StopLine::ab_StopLine_GetDistances(ab_Global::MakeSphericalBearingState(SourceLongitude, SourcePolarAngle, BearingDelta), ForwardDistance, BackwardDistance);
        return ForwardDistance < Distance;
    }

    void ab_StopLine_Load(EC_Buf::TBufEC* Buffer) {
        std::int32_t Index{};
        std::int32_t FirstIndex{};
        std::int32_t LastIndex{};
        PabStopPoint Point{};
        PabStopLine Line{};
        ab_StopLine::ab_StopLine_Clear();
        ab_StopLine::ab_StopPoint_Clear();
        std::int32_t Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Point = ab_StopLine::ab_StopPoint_Add();
            Point->Longitude = EC_Buf::TBufEC_GetSingle(Buffer);
            Point->PolarAngle = EC_Buf::TBufEC_GetSingle(Buffer);
            Point->Radius = EC_Buf::TBufEC_GetSingle(Buffer);
            ab_StopLine::ab_StopPoint_UpdatePosition(Point);
        }
        ab_StopLine::ab_StopPoint_BuildIndex();
        Count = EC_Buf::TBufEC_GetInt32(Buffer);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Line = ab_StopLine::ab_StopLine_Add();
            FirstIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            LastIndex = EC_Buf::TBufEC_GetInt32(Buffer);
            Line->First = StopPointIndex[FirstIndex];
            Line->Last = StopPointIndex[LastIndex];
            Line->Visible = EC_Buf::TBufEC_GetBoolean(Buffer);
            Line->Collidable = EC_Buf::TBufEC_GetBoolean(Buffer);
            if (Line->Visible) {
                {
                    std::int32_t cpp_right = EC_Buf::TBufEC_GetInt32(Buffer);
                    Line->FirstColor = reinterpret_cast<System::PCardinal>(static_cast<std::uint8_t*>(ab_Global::ArcadeMapColorBuffer->Data) + cpp_right);
                }
                {
                    std::int32_t cpp_right_2 = EC_Buf::TBufEC_GetInt32(Buffer);
                    Line->LastColor = reinterpret_cast<System::PCardinal>(static_cast<std::uint8_t*>(ab_Global::ArcadeMapColorBuffer->Data) + cpp_right_2);
                }
            }
        }
    }

} // namespace ab_StopLine
