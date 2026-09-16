#include "layout/aVector.hpp"
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"
#include "units/aVector.hpp"

// Inferred aVector ownership: the contiguous 2D geometry/polygon contribution
// used by galaxy geometry; native PACKAGEINFO visits aVector in the model graph.
namespace aVector {
    EC_Struct::TPointF PerpendicularVector(EC_Struct::TPointF Point) {
        EC_Struct::TPointF Result{};
        Result.X = -Point.Y;
        Result.Y = Point.X;
        return Result;
    }

    float DotProductF(EC_Struct::TPointF Left, EC_Struct::TPointF Right) {
        return static_cast<long double>(Left.X) * Right.X + static_cast<long double>(Left.Y) * Right.Y;
    }

    float VectorLengthF(EC_Struct::TPointF Point) {
        return System::Sqrt(aVector::DotProductF(Point, Point));
    }

    std::uint8_t IsRightOfDirectedLine(EC_Struct::TPointF Point, EC_Struct::TPointF Origin, EC_Struct::TPointF Direction) {
        EC_Struct::TPointF Normal{};
        EC_Struct::TPointF Offset{};
        Normal = aVector::PerpendicularVector(Direction);
        Offset = aVector::MakeVectorF(static_cast<long double>(Point.X) - Origin.X, static_cast<long double>(Point.Y) - Origin.Y);
        return aVector::DotProductF(Offset, Normal) < 0.0L;
    }

    std::uint8_t IsLeftOfDirectedLine(EC_Struct::TPointF Point, EC_Struct::TPointF Origin, EC_Struct::TPointF Direction) {
        EC_Struct::TPointF Normal{};
        EC_Struct::TPointF Offset{};
        Normal = aVector::PerpendicularVector(Direction);
        Offset = aVector::MakeVectorF(static_cast<long double>(Point.X) - Origin.X, static_cast<long double>(Point.Y) - Origin.Y);
        return aVector::DotProductF(Offset, Normal) > 0.0L;
    }

    EC_Struct::TPointF MakeVectorF(float X, float Y) {
        EC_Struct::TPointF Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

    EC_Struct::TPointF VectorBetweenPoints(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        EC_Struct::TPointF Result{};
        Result.X = static_cast<long double>(Last.X) - First.X;
        Result.Y = static_cast<long double>(Last.Y) - First.Y;
        return Result;
    }

    void GetLineEquation(EC_Struct::TPointF First, EC_Struct::TPointF Last, float& A, float& B, float& C) {
        if (std::fabs(static_cast<long double>(First.X) - Last.X) < std::fabs(static_cast<long double>(First.Y) - Last.Y)) {
            A = 1.0f;
            B = pas::real_divide((static_cast<long double>(First.X) - Last.X) * A, static_cast<long double>(Last.Y) - First.Y);
            C = static_cast<long double>(-A) * First.X - static_cast<long double>(B) * First.Y;
        } else {
            B = 1.0f;
            A = pas::real_divide((static_cast<long double>(First.Y) - Last.Y) * B, static_cast<long double>(Last.X) - First.X);
            C = static_cast<long double>(-A) * First.X - static_cast<long double>(B) * First.Y;
        }
    }

    EC_Struct::TPointF IntersectLinesF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2) {
        EC_Struct::TPointF Result{};
        float A1{};
        float B1{};
        float C1{};
        float A2{};
        float B2{};
        float C2{};
        aVector::GetLineEquation(First1, Last1, A1, B1, C1);
        aVector::GetLineEquation(First2, Last2, A2, B2, C2);
        if (static_cast<long double>(A2) * B1 - static_cast<long double>(A1) * B2 == 0.0L) {
            Result.X = 1.0E+20f;
        } else {
            Result.X = pas::real_divide(static_cast<long double>(B2) * C1 - static_cast<long double>(B1) * C2, static_cast<long double>(A2) * B1 - static_cast<long double>(A1) * B2);
        }
        if (static_cast<long double>(A1) * B2 - static_cast<long double>(A2) * B1 == 0.0L) {
            Result.Y = 1.0E+20f;
        } else {
            Result.Y = pas::real_divide(static_cast<long double>(A2) * C1 - static_cast<long double>(A1) * C2, static_cast<long double>(A1) * B2 - static_cast<long double>(A2) * B1);
        }
        return Result;
    }

    std::uint8_t IntersectSegmentWithLine(EC_Struct::TPointF First, EC_Struct::TPointF Last, float A, float B, float C, EC_Struct::TPointF& Intersection) {
        float LineA{};
        float LineB{};
        float LineC{};
        EC_Struct::TPointF Point{};
        aVector::GetLineEquation(First, Last, LineA, LineB, LineC);
        std::uint8_t Result = false;
        if (std::fabs(static_cast<long double>(LineA) - A) < 1.0E-4L && std::fabs(static_cast<long double>(LineB) - B) < 1.0E-4L) {
            return Result;
        }
        Point.X = pas::real_divide(static_cast<long double>(B) * LineC - static_cast<long double>(LineB) * C, static_cast<long double>(A) * LineB - static_cast<long double>(LineA) * B);
        Point.Y = pas::real_divide(static_cast<long double>(A) * LineC - static_cast<long double>(LineA) * C, static_cast<long double>(LineA) * B - static_cast<long double>(A) * LineB);
        if (aVector::DotProductF(aVector::VectorBetweenPoints(Point, First), aVector::VectorBetweenPoints(Point, Last)) < 0.0L || aVector::PointsNearlyEqualF(Point, First) || aVector::PointsNearlyEqualF(Point, Last)) {
            Result = true;
            Intersection = Point;
        }
        return Result;
    }

    std::uint8_t IntersectSegmentWithDirectedLine(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF LineFirst, EC_Struct::TPointF LineLast, EC_Struct::TPointF& Intersection) {
        EC_Struct::TPointF Direction{};
        Direction = aVector::VectorBetweenPoints(LineFirst, LineLast);
        if (aVector::IsLeftOfDirectedLine(First, LineFirst, Direction) != aVector::IsLeftOfDirectedLine(Last, LineFirst, Direction)) {
            Intersection = aVector::IntersectLinesF(First, Last, LineFirst, LineLast);
            return true;
        }
        return false;
    }

    std::uint8_t IntersectSegmentsF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2, EC_Struct::TPointF& Intersection) {
        std::uint8_t Result = true;
        if (static_cast<std::uint8_t>(aVector::IntersectSegmentWithDirectedLine(First1, Last1, First2, Last2, Intersection) ^ 1) || static_cast<std::uint8_t>(aVector::IntersectSegmentWithDirectedLine(First2, Last2, First1, Last1, Intersection) ^ 1)) {
            return false;
        }
        return Result;
    }

    TRectF MakeRectF(float Left, float Top, float Right, float Bottom) {
        TRectF Result{};
        Result.Left = Left;
        Result.Top = Top;
        Result.Right = Right;
        Result.Bottom = Bottom;
        return Result;
    }

    TRectF RectFromPointsF(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        TRectF Result{};
        Result.Left = First.X;
        Result.Top = First.Y;
        Result.Right = Last.X;
        Result.Bottom = Last.Y;
        return Result;
    }

    std::uint8_t PointsNearlyEqualF(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        return std::fabs(static_cast<long double>(First.X) - Last.X) < 0.1L && std::fabs(static_cast<long double>(First.Y) - Last.Y) < 0.1L;
    }

    std::uint8_t SegmentsNearlyEqualF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2) {
        return aVector::PointsNearlyEqualF(First1, First2) && aVector::PointsNearlyEqualF(Last1, Last2) || aVector::PointsNearlyEqualF(First1, Last2) && aVector::PointsNearlyEqualF(Last1, First2);
    }

    std::uint8_t ScalarsNearlyEqualF(float First, float Last) {
        return std::fabs(static_cast<long double>(First) - Last) < 0.1L;
    }

    float PointDistanceF(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        return aVector::VectorLengthF(aVector::VectorBetweenPoints(First, Last));
    }

    float PointSegmentDistanceF(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF Point) {
        EC_Struct::TPointF Normal{};
        EC_Struct::TPointF Intersection{};
        Normal = aVector::PerpendicularVector(aVector::VectorBetweenPoints(First, Last));
        if (aVector::IntersectSegmentWithDirectedLine(First, Last, Point, EC_Struct::MakePointF(static_cast<long double>(Point.X) + Normal.X, static_cast<long double>(Point.Y) + Normal.Y), Intersection)) {
            return aVector::PointDistanceF(Point, Intersection);
        }
        float FirstDistance = aVector::PointDistanceF(First, Point);
        float LastDistance = aVector::PointDistanceF(Last, Point);
        if (FirstDistance > LastDistance) {
            return LastDistance;
        }
        return FirstDistance;
    }

    std::int32_t ClassifyPointToSegment(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF Point) {
        EC_Struct::TPointF Direction{};
        EC_Struct::TPointF Offset{};
        Direction = EC_Struct::MakePointF(static_cast<long double>(Last.X) - First.X, static_cast<long double>(Last.Y) - First.Y);
        Offset = EC_Struct::MakePointF(static_cast<long double>(Point.X) - First.X, static_cast<long double>(Point.Y) - First.Y);
        float Cross = static_cast<long double>(Direction.X) * Offset.Y - static_cast<long double>(Direction.Y) * Offset.X;
        if (Cross > 0.0L) {
            return 1;
        } else if (Cross < 0.0L) {
            return 2;
        } else if (static_cast<long double>(Direction.X) * Offset.X < 0.0L || static_cast<long double>(Direction.Y) * Offset.Y < 0.0L) {
            return 3;
        } else {
            pas::Extended cpp_left = System::Sqrt(static_cast<long double>(Direction.X) * Direction.X + static_cast<long double>(Direction.Y) * Direction.Y);
            if (cpp_left < System::Sqrt(static_cast<long double>(Offset.X) * Offset.X + static_cast<long double>(Offset.Y) * Offset.Y)) {
                return 4;
            } else if (First.X == Point.X && First.Y == Point.Y) {
                return 6;
            } else if (Last.X == Point.X && Last.Y == Point.Y) {
                return 7;
            } else {
                return 5;
            }
        }
    }

    void TPolygon2D_Create(TPolygon2D* Self) {
        Self->Next = nullptr;
        Self->Previous = nullptr;
        Self->AreaValid = false;
        Self->Flag39 = false;
        Self->CachedArea = 0.0f;
        Self->Points = pas::make_object<pas::List>();
        Self->GroupId = -1;
        Self->Unknown14 = -1;
        Self->Extent = EC_Struct::MakePointF(0.0f, 0.0f);
        Self->Bounds = aVector::MakeRectF(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void TPolygon2D_CreateTriangle(TPolygon2D* Self, EC_Struct::TPointF A, EC_Struct::TPointF B, EC_Struct::TPointF C) {
        Self->Next = nullptr;
        Self->Previous = nullptr;
        Self->AreaValid = false;
        Self->Flag39 = false;
        Self->CachedArea = 0.0f;
        Self->Points = pas::make_object<pas::List>();
        Self->GroupId = -1;
        Self->Unknown14 = -1;
        Self->Extent = EC_Struct::MakePointF(0.0f, 0.0f);
        Self->Bounds = aVector::MakeRectF(0.0f, 0.0f, 0.0f, 0.0f);
        Self->SetTriangle(A, B, C);
    }

    void TPolygon2D_Destroy(TPolygon2D* Self) {
        Self->Clear();
        pas::free(Self->Points);
        if (Self->Next != nullptr) {
            pas::free(Self->Next);
        }
    }

    void TPolygon2D::Clear() {
        std::int32_t Index{};
        EC_Struct::PPointF Point{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
            Point = pas::list_at<EC_Struct::TPointF>(Points, Index);
            pas::dispose(Point);
        }
        pas::list_clear(Points);
        GroupId = -1;
        Unknown14 = -1;
        Extent = EC_Struct::MakePointF(0.0f, 0.0f);
        Bounds = aVector::MakeRectF(0.0f, 0.0f, 0.0f, 0.0f);
        AreaValid = false;
        Flag39 = false;
    }

    void TPolygon2D::SetRectangle(Types::TRect Rect) {
        EC_Struct::PPointF Point{};
        Clear();
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = Rect.Left;
        Point->Y = Rect.Top;
        pas::list_add(Points, static_cast<void*>(Point));
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = Rect.Right;
        Point->Y = Rect.Top;
        pas::list_add(Points, static_cast<void*>(Point));
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = Rect.Right;
        Point->Y = Rect.Bottom;
        pas::list_add(Points, static_cast<void*>(Point));
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = Rect.Left;
        Point->Y = Rect.Bottom;
        pas::list_add(Points, static_cast<void*>(Point));
        Bounds = aVector::MakeRectF(Rect.Left, Rect.Top, Rect.Right, Rect.Bottom);
        Extent = EC_Struct::MakePointF(Rect.Right - Rect.Left, Rect.Bottom - Rect.Top);
        AreaValid = false;
        Flag39 = false;
    }

    // Takes ownership of the list and its PPointF entries.
    void TPolygon2D::TakePoints(pas::List* NewPoints) {
        Clear();
        pas::free(Points);
        Points = NewPoints;
        AreaValid = false;
        Flag39 = false;
        RecalculateBounds();
    }

    void TPolygon2D::SetTriangle(EC_Struct::TPointF A, EC_Struct::TPointF B, EC_Struct::TPointF C) {
        EC_Struct::PPointF Point{};
        Clear();
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = A.X;
        Point->Y = A.Y;
        pas::list_add(Points, static_cast<void*>(Point));
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = B.X;
        Point->Y = B.Y;
        pas::list_add(Points, static_cast<void*>(Point));
        pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Point->X = C.X;
        Point->Y = C.Y;
        pas::list_add(Points, static_cast<void*>(Point));
        AreaValid = false;
        Flag39 = false;
        RecalculateBounds();
    }

    void TPolygon2D::RecalculateBounds() {
        std::int32_t Index{};
        if (pas::list_count(Points) == 0) {
            return;
        }
        EC_Struct::PPointF Point = pas::list_at<EC_Struct::TPointF>(Points, 0);
        Bounds = aVector::RectFromPointsF(pas::load_unaligned<EC_Struct::TPointF>(Point), pas::load_unaligned<EC_Struct::TPointF>(Point));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
            Point = pas::list_at<EC_Struct::TPointF>(Points, Index);
            if (Point->X < Bounds.Left) {
                Bounds.Left = Point->X;
            }
            if (Point->X > Bounds.Right) {
                Bounds.Right = Point->X;
            }
            if (Point->Y < Bounds.Top) {
                Bounds.Top = Point->Y;
            }
            if (Point->Y > Bounds.Bottom) {
                Bounds.Bottom = Point->Y;
            }
        }
        Extent = EC_Struct::MakePointF(static_cast<long double>(Bounds.Right) - Bounds.Left, static_cast<long double>(Bounds.Bottom) - Bounds.Top);
        GetArea();
    }

    // Appends at the tail and sets Polygon.Previous; requires nonnil Polygon.
    void TPolygon2D::Append(TPolygon2D* Polygon) {
        TPolygon2D* Tail{};
        if (Next == nullptr) {
            Next = Polygon;
            Polygon->Previous = this;
        } else {
            Tail = this;
            while (Tail->Next != nullptr) {
                Tail = Tail->Next;
            }
            Tail->Append(Polygon);
        }
    }

    void TPolygon2D::InsertAfter(TPolygon2D* Polygon) {
        Polygon->Previous = this;
        Polygon->Next = Next;
        if (Next != nullptr) {
            Next->Previous = Polygon;
        }
        Next = Polygon;
    }

    void TPolygon2D::SplitChainByLine(float A, float B, float C) {
        std::int32_t Index{};
        EC_Struct::PPointF Point{};
        EC_Struct::PPointF Last{};
        std::int32_t FirstIndex{};
        std::int32_t LastIndex{};
        EC_Struct::TPointF FirstIntersection{};
        EC_Struct::TPointF LastIntersection{};
        EC_Struct::TPointF Intersection{};
        TPolygon2D* NewPolygon{};
        TPolygon2D* Following{};
        pas::List* FirstPoints{};
        pas::List* LastPoints{};
        TPolygon2D* Current = this;
        while (Current != nullptr) {
            FirstIndex = -1;
            LastIndex = -1;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Current->Points) - 1); cpp_range.next(Index); ) {
                Point = pas::list_at<EC_Struct::TPointF>(Current->Points, Index);
                if (Index == pas::list_count(Current->Points) - 1) {
                    Last = pas::list_at<EC_Struct::TPointF>(Current->Points, 0);
                } else {
                    Last = pas::list_at<EC_Struct::TPointF>(Current->Points, Index + 1);
                }
                if (aVector::IntersectSegmentWithLine(pas::load_unaligned<EC_Struct::TPointF>(Point), pas::load_unaligned<EC_Struct::TPointF>(Last), A, B, C, Intersection) && static_cast<std::uint8_t>(aVector::PointsNearlyEqualF(pas::load_unaligned<EC_Struct::TPointF>(Last), Intersection) ^ 1)) {
                    if (FirstIndex == -1) {
                        FirstIndex = Index;
                        FirstIntersection = Intersection;
                    } else if (LastIndex == -1) {
                        LastIndex = Index;
                        LastIntersection = Intersection;
                    } else {
                        // Native is this ANSI literal, not the former IDA nullsub_13.
                        pas::raise(pas::make_exception<pas::Exception>("\303\353\376\352! \313\350\355\350\377 \357\345\360\345\361\345\352\340\345\362 \357\356\353\350\343\356\355 \342 \362\360\345\365 \362\356\367\352\340\365"_a));
                    }
                }
            }
            if (FirstIndex > -1 && LastIndex > -1) {
                NewPolygon = pas::construct_call<TPolygon2D>(TPolygon2D_Create);
                FirstPoints = pas::make_object<pas::List>();
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, FirstIndex); cpp_range_2.next(Index); ) {
                    Point = pas::list_at<EC_Struct::TPointF>(Current->Points, Index);
                    pas::list_add(FirstPoints, static_cast<void*>(Point));
                }
                Last = pas::list_at<EC_Struct::TPointF>(Current->Points, FirstIndex);
                if (!aVector::PointsNearlyEqualF(pas::load_unaligned<EC_Struct::TPointF>(Last), FirstIntersection)) {
                    pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                    Point->X = FirstIntersection.X;
                    Point->Y = FirstIntersection.Y;
                    pas::list_add(FirstPoints, static_cast<void*>(Point));
                }
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                Point->X = LastIntersection.X;
                Point->Y = LastIntersection.Y;
                pas::list_add(FirstPoints, static_cast<void*>(Point));
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(LastIndex + 1, pas::list_count(Current->Points) - 1); cpp_range_3.next(Index); ) {
                    Point = pas::list_at<EC_Struct::TPointF>(Current->Points, Index);
                    pas::list_add(FirstPoints, static_cast<void*>(Point));
                }
                LastPoints = pas::make_object<pas::List>();
                Last = pas::list_at<EC_Struct::TPointF>(Current->Points, LastIndex);
                if (!aVector::PointsNearlyEqualF(LastIntersection, pas::load_unaligned<EC_Struct::TPointF>(Last))) {
                    pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                    Point->X = LastIntersection.X;
                    Point->Y = LastIntersection.Y;
                    pas::list_add(LastPoints, static_cast<void*>(Point));
                }
                pas::get_mem_at(&Point, static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
                Point->X = FirstIntersection.X;
                Point->Y = FirstIntersection.Y;
                pas::list_add(LastPoints, static_cast<void*>(Point));
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(FirstIndex + 1, LastIndex); cpp_range_4.next(Index); ) {
                    Point = pas::list_at<EC_Struct::TPointF>(Current->Points, Index);
                    pas::list_add(LastPoints, static_cast<void*>(Point));
                }
                NewPolygon->TakePoints(FirstPoints);
                NewPolygon->GroupId = GroupId;
                NewPolygon->Flag39 = false;
                pas::free(Current->Points);
                Current->Points = LastPoints;
                Current->RecalculateBounds();
                Current->AreaValid = false;
                Current->Flag39 = false;
                Following = Current->Next;
                Current->InsertAfter(NewPolygon);
                Current = Following;
            } else {
                Current = Current->Next;
            }
        }
    }

    void TPolygon2D::SplitChainByPoints(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        float A{};
        float B{};
        float C{};
        aVector::GetLineEquation(First, Last, A, B, C);
        SplitChainByLine(A, B, C);
    }

    TPolygon2D* TPolygon2D::ExtractFollowingGroup(std::int32_t Id) {
        TPolygon2D* Removed{};
        Flag39 = false;
        AreaValid = false;
        TPolygon2D* Head = nullptr;
        TPolygon2D* Current = Next;
        while (Current != nullptr) {
            if (Current->GroupId == Id) {
                Removed = Current;
                Current = Current->Next;
                if (Removed->Previous != nullptr) {
                    Removed->Previous->Next = Removed->Next;
                }
                if (Removed->Next != nullptr) {
                    Removed->Next->Previous = Removed->Previous;
                }
                Removed->Next = nullptr;
                Removed->Previous = nullptr;
                if (Head == nullptr) {
                    Head = Removed;
                } else {
                    Head->Append(Removed);
                }
            } else {
                Current = Current->Next;
            }
        }
        return Head;
    }

    std::uint8_t TPolygon2D::ContainsPoint(EC_Struct::TPointF Point) {
        std::int32_t Index{};
        EC_Struct::PPointF First{};
        EC_Struct::PPointF Last{};
        std::uint8_t Result = false;
        if (Point.X >= Bounds.Left && Point.X <= Bounds.Right && Point.Y >= Bounds.Top && Point.Y <= Bounds.Bottom) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
                First = pas::list_at<EC_Struct::TPointF>(Points, Index);
                if (Index == pas::list_count(Points) - 1) {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
                } else {
                    Last = pas::list_at<EC_Struct::TPointF>(Points, Index + 1);
                }
                if (aVector::IsRightOfDirectedLine(Point, pas::load_unaligned<EC_Struct::TPointF>(First), aVector::VectorBetweenPoints(pas::load_unaligned<EC_Struct::TPointF>(First), pas::load_unaligned<EC_Struct::TPointF>(Last)))) {
                    return Result;
                }
            }
            return true;
        }
        return Result;
    }

    std::uint8_t TPolygon2D::ChainContainsPoint(EC_Struct::TPointF Point) {
        std::uint8_t Result = true;
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            if (Polygon->ContainsPoint(Point)) {
                return Result;
            }
            Polygon = Polygon->Next;
        }
        return false;
    }

    TPolygon2D* TPolygon2D::FindContainingPolygon(EC_Struct::TPointF Point) {
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            if (Polygon->ContainsPoint(Point)) {
                break;
            }
            Polygon = Polygon->Next;
        }
        return Polygon;
    }

    std::uint8_t TPolygon2D::AssignGroupAtPoint(EC_Struct::TPointF Point, std::int32_t Id) {
        std::uint8_t Result = false;
        TPolygon2D* Polygon = FindContainingPolygon(Point);
        if (Polygon != nullptr) {
            if (Polygon->GroupId == Id) {
                Result = true;
            }
            if (Polygon->GroupId == -1) {
                Result = true;
                Polygon->GroupId = Id;
            }
        }
        return Result;
    }

    pas::List* TPolygon2D::ExtractBoundaryEdges() {
        pas::List* Edges{};
        pas::List* Boundary = pas::make_object<pas::List>();
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            Edges = Polygon->ExtractEdges();
            Boundary = TPolygon2D::MergeUnsharedEdges(Boundary, Edges);
            Polygon = Polygon->Next;
        }
        return Boundary;
    }

    // Consumes both lists and frees their edge records.
    pas::List* TPolygon2D::MergeUnsharedEdges(pas::List* First, pas::List* Second) {
        std::int32_t Index{};
        std::int32_t OtherIndex{};
        PPolygonEdge Source{};
        PPolygonEdge Edge{};
        std::uint8_t Found{};
        pas::List* Edges = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(First) - 1); cpp_range.next(Index); ) {
            Source = pas::list_at<TPolygonEdge>(First, Index);
            Found = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Second) - 1); cpp_range_2.next(OtherIndex); ) {
                Edge = pas::list_at<TPolygonEdge>(Second, OtherIndex);
                if (aVector::SegmentsNearlyEqualF(Source->First, Source->Last, Edge->First, Edge->Last)) {
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                pas::get_mem_at(&Edge, static_cast<std::int32_t>(sizeof(TPolygonEdge)));
                pas::store_unaligned<EC_Struct::TPointF>(&Edge->First, Source->First);
                pas::store_unaligned<EC_Struct::TPointF>(&Edge->Last, Source->Last);
                pas::list_add(Edges, static_cast<void*>(Edge));
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Second) - 1); cpp_range_3.next(Index); ) {
            Source = pas::list_at<TPolygonEdge>(Second, Index);
            Found = false;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(First) - 1); cpp_range_4.next(OtherIndex); ) {
                Edge = pas::list_at<TPolygonEdge>(First, OtherIndex);
                if (aVector::SegmentsNearlyEqualF(Source->First, Source->Last, Edge->First, Edge->Last)) {
                    Found = true;
                    break;
                }
            }
            if (!Found) {
                pas::get_mem_at(&Edge, static_cast<std::int32_t>(sizeof(TPolygonEdge)));
                pas::store_unaligned<EC_Struct::TPointF>(&Edge->First, Source->First);
                pas::store_unaligned<EC_Struct::TPointF>(&Edge->Last, Source->Last);
                pas::list_add(Edges, static_cast<void*>(Edge));
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(First) - 1); cpp_range_5.next(Index); ) {
            Source = pas::list_at<TPolygonEdge>(First, Index);
            pas::dispose(Source);
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Second) - 1); cpp_range_6.next(Index); ) {
            Source = pas::list_at<TPolygonEdge>(Second, Index);
            pas::dispose(Source);
        }
        pas::free(First);
        pas::free(Second);
        return Edges;
    }

    // Caller owns the list and its PPolygonEdge entries.
    pas::List* TPolygon2D::ExtractEdges() {
        PPolygonEdge Edge{};
        EC_Struct::PPointF First{};
        EC_Struct::PPointF Last{};
        std::int32_t Index{};
        pas::List* Edges = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
            First = pas::list_at<EC_Struct::TPointF>(Points, Index);
            if (Index == pas::list_count(Points) - 1) {
                Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
            } else {
                Last = pas::list_at<EC_Struct::TPointF>(Points, Index + 1);
            }
            pas::get_mem_at(&Edge, static_cast<std::int32_t>(sizeof(TPolygonEdge)));
            pas::store_unaligned<EC_Struct::TPointF>(&Edge->First, pas::load_unaligned<EC_Struct::TPointF>(First));
            pas::store_unaligned<EC_Struct::TPointF>(&Edge->Last, pas::load_unaligned<EC_Struct::TPointF>(Last));
            pas::list_add(Edges, static_cast<void*>(Edge));
        }
        return Edges;
    }

    void TPolygon2D::ResetChainGroups() {
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            Polygon->GroupId = -1;
            Polygon->Unknown14 = -1;
            Polygon = Polygon->Next;
        }
    }

    // Includes Self; nil returns zero.
    std::int32_t TPolygon2D::CountChain() {
        std::int32_t Result = 0;
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            ++Result;
            Polygon = Polygon->Next;
        }
        return Result;
    }

    TPolygon2D* TPolygon2D::GetChainItem(std::int32_t Index) {
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            if (Index <= 0) {
                break;
            }
            --Index;
            Polygon = Polygon->Next;
        }
        return Polygon;
    }

    // The first uncached call fills CachedArea but returns zero; later calls return the cache.
    float TPolygon2D::GetArea() {
        std::int32_t Index{};
        EC_Struct::PPointF TriangleFirst{};
        EC_Struct::PPointF Middle{};
        EC_Struct::PPointF Last{};
        float A{};
        float B{};
        float C{};
        float Height{};
        float Square{};
        if (AreaValid) {
            return CachedArea;
        }
        float Result = 0.0f;
        CachedArea = 0.0f;
        if (pas::list_count(Points) >= 3) {
            // Preserve DCC32's receiver-before-index argument order; + 0 emits no arithmetic.
            TriangleFirst = pas::list_at<EC_Struct::TPointF>(reinterpret_cast<pas::List*>(reinterpret_cast<std::uint8_t*>(Points) + 0), 0);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(Points) - 2); cpp_range.next(Index); ) {
                Middle = pas::list_at<EC_Struct::TPointF>(reinterpret_cast<pas::List*>(reinterpret_cast<std::uint8_t*>(Points) + 0), Index);
                Last = pas::list_at<EC_Struct::TPointF>(Points, Index + 1);
                A = aVector::PointDistanceF(pas::load_unaligned<EC_Struct::TPointF>(TriangleFirst), pas::load_unaligned<EC_Struct::TPointF>(Middle));
                B = aVector::PointDistanceF(pas::load_unaligned<EC_Struct::TPointF>(TriangleFirst), pas::load_unaligned<EC_Struct::TPointF>(Last));
                C = aVector::PointDistanceF(pas::load_unaligned<EC_Struct::TPointF>(Middle), pas::load_unaligned<EC_Struct::TPointF>(Last));
                if (A == 0.0L) {
                    A = 1.0f;
                }
                Square = static_cast<long double>(A) * A + static_cast<long double>(B) * B - static_cast<long double>(C) * C;
                if (Square < 0.0L) {
                    Square = 0.01f;
                }
                Square = static_cast<long double>(B) * B - pas::real_divide(pas::sqr(static_cast<pas::Extended>(Square)), 4.0L * A * A);
                if (Square < 0.0L) {
                    Square = 0.01f;
                }
                Height = System::Sqrt(Square);
                CachedArea = 0.5L * Height * A + CachedArea;
            }
            AreaValid = true;
        }
        return Result;
    }

    float TPolygon2D::GetChainArea() {
        float Result = 0.0f;
        TPolygon2D* Polygon = this;
        while (Polygon != nullptr) {
            Result = static_cast<long double>(Polygon->GetArea()) + Result;
            Polygon = Polygon->Next;
        }
        return Result;
    }

    std::uint8_t TPolygon2D::IntersectsPolygon(TPolygon2D* Polygon) {
        std::int32_t Index{};
        EC_Struct::PPointF First{};
        EC_Struct::PPointF Last{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
            First = pas::list_at<EC_Struct::TPointF>(Points, Index);
            if (Index == pas::list_count(Points) - 1) {
                Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
            } else {
                Last = pas::list_at<EC_Struct::TPointF>(Points, Index + 1);
            }
            if (Polygon->IntersectsSegment(pas::load_unaligned<EC_Struct::TPointF>(First), pas::load_unaligned<EC_Struct::TPointF>(Last))) {
                return true;
            }
        }
        return false;
    }

    std::uint8_t TPolygon2D::IntersectsEdge(PPolygonEdge Edge) {
        std::int32_t Index{};
        EC_Struct::PPointF First{};
        EC_Struct::PPointF Last{};
        EC_Struct::TPointF Intersection{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Points) - 1); cpp_range.next(Index); ) {
            First = pas::list_at<EC_Struct::TPointF>(Points, Index);
            if (Index == pas::list_count(Points) - 1) {
                Last = pas::list_at<EC_Struct::TPointF>(Points, 0);
            } else {
                Last = pas::list_at<EC_Struct::TPointF>(Points, Index + 1);
            }
            if (aVector::IntersectSegmentsF(pas::load_unaligned<EC_Struct::TPointF>(First), pas::load_unaligned<EC_Struct::TPointF>(Last), Edge->First, Edge->Last, Intersection) && static_cast<std::uint8_t>(aVector::PointsNearlyEqualF(pas::load_unaligned<EC_Struct::TPointF>(First), Intersection) ^ 1) && static_cast<std::uint8_t>(aVector::PointsNearlyEqualF(pas::load_unaligned<EC_Struct::TPointF>(Last), Intersection) ^ 1)) {
                return true;
            }
        }
        return false;
    }

    std::uint8_t TPolygon2D::IntersectsSegment(EC_Struct::TPointF First, EC_Struct::TPointF Last) {
        TPolygonEdge Edge{};
        Edge.First = First;
        Edge.Last = Last;
        return IntersectsEdge(&Edge);
    }

    std::uint8_t TPolygon2D::ChainSelfIntersects() {
        TPolygon2D* Last{};
        TPolygon2D* First = this;
        while (First != nullptr) {
            Last = First->Next;
            while (Last != nullptr) {
                if (First != Last && First->IntersectsPolygon(Last)) {
                    return true;
                }
                Last = Last->Next;
            }
            First = First->Next;
        }
        return false;
    }

    std::uint8_t TPolygon2D::IntersectsChain(TPolygon2D* Polygon) {
        TPolygon2D* Last{};
        TPolygon2D* First = this;
        while (First != nullptr) {
            Last = Polygon;
            while (Last != nullptr) {
                if (First != Last && First->IntersectsPolygon(Last)) {
                    return true;
                }
                Last = Last->Next;
            }
            First = First->Next;
        }
        return false;
    }

    void TPolygon2D::p_destroy() {
        aVector::TPolygon2D_Destroy(this);
    }

} // namespace aVector
