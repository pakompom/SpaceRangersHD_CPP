#pragma once
#include "types/EC_Struct.hpp"
#include "types/aVector.hpp"

namespace aVector {
    EC_Struct::TPointF PerpendicularVector(EC_Struct::TPointF Point);

    float DotProductF(EC_Struct::TPointF Left, EC_Struct::TPointF Right);

    float VectorLengthF(EC_Struct::TPointF Point);

    std::uint8_t IsRightOfDirectedLine(EC_Struct::TPointF Point, EC_Struct::TPointF Origin, EC_Struct::TPointF Direction);

    std::uint8_t IsLeftOfDirectedLine(EC_Struct::TPointF Point, EC_Struct::TPointF Origin, EC_Struct::TPointF Direction);

    EC_Struct::TPointF MakeVectorF(float X, float Y);

    EC_Struct::TPointF VectorBetweenPoints(EC_Struct::TPointF First, EC_Struct::TPointF Last);

    void GetLineEquation(EC_Struct::TPointF First, EC_Struct::TPointF Last, float& A, float& B, float& C);

    EC_Struct::TPointF IntersectLinesF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2);

    std::uint8_t IntersectSegmentWithLine(EC_Struct::TPointF First, EC_Struct::TPointF Last, float A, float B, float C, EC_Struct::TPointF& Intersection);

    std::uint8_t IntersectSegmentWithDirectedLine(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF LineFirst, EC_Struct::TPointF LineLast, EC_Struct::TPointF& Intersection);

    std::uint8_t IntersectSegmentsF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2, EC_Struct::TPointF& Intersection);

    TRectF MakeRectF(float Left, float Top, float Right, float Bottom);

    TRectF RectFromPointsF(EC_Struct::TPointF First, EC_Struct::TPointF Last);

    std::uint8_t PointsNearlyEqualF(EC_Struct::TPointF First, EC_Struct::TPointF Last);

    std::uint8_t SegmentsNearlyEqualF(EC_Struct::TPointF First1, EC_Struct::TPointF Last1, EC_Struct::TPointF First2, EC_Struct::TPointF Last2);

    std::uint8_t ScalarsNearlyEqualF(float First, float Last);

    float PointDistanceF(EC_Struct::TPointF First, EC_Struct::TPointF Last);

    float PointSegmentDistanceF(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF Point);

    std::int32_t ClassifyPointToSegment(EC_Struct::TPointF First, EC_Struct::TPointF Last, EC_Struct::TPointF Point);

    void TPolygon2D_Create(TPolygon2D* Self);

    void TPolygon2D_CreateTriangle(TPolygon2D* Self, EC_Struct::TPointF A, EC_Struct::TPointF B, EC_Struct::TPointF C);

    void TPolygon2D_Destroy(TPolygon2D* Self);

} // namespace aVector
