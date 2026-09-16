#pragma once
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/aMyFunction.hpp"

namespace aMyFunction {
    extern pas::WideString InfoNameColorTag;

    extern pas::WideString InfoHullSeriesColorTag;

    extern const float PolarDegreesToRadians;

    std::int32_t RandomIntRange(std::int32_t BoundA, std::int32_t BoundB);

    std::int32_t SeededRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t Seed);

    float RandomUnitFloat();

    float SeededRandomUnitFloat(std::uint32_t Seed);

    double RandomFloatRange(double BoundA, double BoundB);

    double SeededRandomFloatRange(std::uint32_t Seed, double BoundA, double BoundB);

    std::uint32_t StepRandomSeed(std::uint32_t Seed);

    std::uint32_t AdvanceRandomSeed(std::uint32_t& Seed);

    std::int32_t NextRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t& Seed);

    double NextRandomFloatRange(double BoundA, double BoundB, std::uint32_t& Seed);

    double NextRandomUnitFloat(std::uint32_t& Seed);

    double RemapClamped(double Value, double InMin, double InMax, double OutMin, double OutMax);

    std::int32_t RoundAndTruncateToTens(double Value);

    float PointDistanceSquared(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    inline EC_Struct::TPointF PolarToPoint(TPolarPoint Polar);

    double PointDistance(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    double RadiansToHeadingDegrees(double Angle);

    double HeadingDegreesToRadians(double Angle);

    double PointBearingDegrees(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    double HeadingDifferenceDegrees(double FromHeading, double ToHeading);

    float WrapHeadingDegrees(float Angle);

    void ReplaceTextToken(pas::WideString& Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag);

    pas::WideString ReplaceColoredToken(pas::WideString Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag);

    pas::WideString FormatText1(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token, pas::WideString Replacement);

    pas::WideString FormatText2(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2);

    pas::WideString FormatText3(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2, pas::WideString Token3, pas::WideString Replacement3);

    pas::WideString WrapTextInColor(pas::WideString Text, pas::WideString ColorTag);

    std::uint8_t RayIntersectsOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF ThroughPoint, EC_Struct::TPointF& Intersection, float Radius);

    std::int32_t DecrementWrappedValue(std::int32_t Value, std::int32_t Minimum, std::int32_t Maximum);

    std::int32_t IncrementWrapped(std::int32_t& Value, std::int32_t Minimum, std::int32_t Maximum);

    double FractionalQuotient(std::int32_t Numerator, std::int32_t Denominator);

    std::int32_t RoundAndTruncateToFives(double Value);

    std::int32_t RoundAndTruncateToHundreds(double Value);

    inline EC_Struct::TPointF PointFromRadiusAngle(float Radius, float Angle);

    inline EC_Struct::TPointF OffsetPointByRadiusAngle(EC_Struct::TPointF Origin, float Radius, float Angle);

    inline EC_Struct::TPointF RotateAndTranslatePoint(EC_Struct::TPointF Point, EC_Struct::TPointF Translation, float Angle);

    TPolarRadiansPoint IntegerPointToPolar(Types::TPoint Point);

    std::uint8_t HeadingDegreesToByte(double Angle);

    double ByteToHeadingDegrees(std::uint8_t Angle);

    float WrapSignedHeadingDegrees(float Angle);

    std::uint8_t HeadingWithinArc(float ArcStart, float Heading, float ArcEnd);

    EC_Struct::TPointF PushPointOutsideCircleBand(EC_Struct::TPointF Point, float Radius, float Margin);

    EC_Struct::TPointF RotatePointQuarterTurn(EC_Struct::TPointF Center, EC_Struct::TPointF Point);

    std::uint8_t IntersectLines(EC_Struct::TPointF A1, EC_Struct::TPointF A2, EC_Struct::TPointF B1, EC_Struct::TPointF B2, EC_Struct::TPointF& Intersection);

    std::uint8_t SegmentIntersectsRectEdges(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF TopLeft, EC_Struct::TPointF BottomRight, EC_Struct::TPointF& Intersection);

    std::uint8_t SegmentIntersectsCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF Center, float Radius);

    std::uint8_t SegmentCrossesOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, float Radius);

    double CalculateTangentArcOffset(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, double Heading, double Angle);

    void CircleTangentPoints(EC_Struct::TPointF Point, float Radius, EC_Struct::TPointF& LeftPoint, EC_Struct::TPointF& RightPoint);

    EC_Struct::TPointF PointBehindHeading(EC_Struct::TPointF Origin, double Heading, double Distance, std::uint32_t Seed);

    std::int32_t IntegerPointDistancePlusOne(Types::TPoint PointA, Types::TPoint PointB);

    EC_Struct::TPointF MakeFloatPoint(std::int32_t X, std::int32_t Y);

    pas::WideString NormalizeTextHighlightColors(pas::WideString Text);

    void TObjectList_Destroy(TObjectList* Self);

} // namespace aMyFunction

#include "inline/aMyFunction.hpp"
