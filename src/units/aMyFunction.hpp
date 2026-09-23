#pragma once
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/aMyFunction.hpp"

namespace aMyFunction {
    // Complete markup tags. Keep these untyped so they remain string literals.
    // Dialogs remap the standard highlight and equipment colors for light panels.
    extern const pas::WideString TextHighlightColorTag;

    extern const pas::WideString DialogHighlightColorTag;

    extern const pas::WideString EquipmentBonusColorTag;

    extern const pas::WideString DialogEquipmentBonusColorTag;

    extern const pas::WideString DialogGreenColorTag;

    extern const pas::WideString EndColorTag;

    // Shared palette; the same colors serve several unrelated display roles.
    // Exact spelling matters to ReplaceAllWideString; padded RGB tags differ.
    extern const pas::WideString RedColorTag;

    extern const pas::WideString GreenColorTag;

    extern const pas::WideString GrayColorTag;

    extern const pas::WideString YellowColorTag;

    extern const pas::WideString BlackColorTag;

    extern const pas::WideString MagentaColorTag;

    extern const pas::WideString CyanColorTag;

    extern const pas::WideString OrangeColorTag;

    extern const pas::WideString GoldColorTag;

    extern const pas::WideString AzureColorTag;

    extern const pas::WideString DarkGreenColorTag;

    extern const pas::WideString BrightBlueColorTag;

    extern const pas::WideString MicroModuleHighPriorityColorTag;

    extern const pas::WideString DefaultInfoNameColorTag;

    extern const pas::WideString DefaultInfoHullSeriesColorTag;

    // Configured by GI_Main from StyleColor.InfoNameColor / InfoHullSeriesColor.
    extern pas::WideString InfoNameColorTag;

    extern pas::WideString InfoHullSeriesColorTag;

    extern const float PolarDegreesToRadians;

    // Integer ranges include both endpoints and accept either endpoint order.
    // Seeded helpers read the supplied seed; Next helpers advance it, except in chaotic mode.
    // Accepts either endpoint order.
    std::int32_t RandomIntRange(std::int32_t BoundA, std::int32_t BoundB);

    // Chaotic mode ignores Seed.
    std::int32_t SeededRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t Seed);

    // One of 1000 discrete values from 0.001 through 1.0 inclusive.
    float RandomUnitFloat();

    // Chaotic mode ignores Seed.
    float SeededRandomUnitFloat(std::uint32_t Seed);

    // Endpoints are quantized as Trunc(bound*1000+1)/1000; results have 0.001 resolution.
    double RandomFloatRange(double BoundA, double BoundB);

    // Uses RandomFloatRange's endpoint quantization; chaotic mode ignores Seed.
    double SeededRandomFloatRange(std::uint32_t Seed, double BoundA, double BoundB);

    std::uint32_t StepRandomSeed(std::uint32_t Seed);

    std::uint32_t AdvanceRandomSeed(std::uint32_t& Seed);

    // Chaotic mode leaves Seed unchanged.
    std::int32_t NextRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t& Seed);

    // Original source calls RndDoubleOut(BoundA, BoundB, FRndOut); see the market_match fixture evidence.
    // Uses RandomFloatRange's endpoint quantization; chaotic mode leaves Seed unchanged.
    double NextRandomFloatRange(double BoundA, double BoundB, std::uint32_t& Seed);

    // Normally in [0,1). Chaotic mode leaves Seed unchanged and instead yields 0.001..1.001.
    double NextRandomUnitFloat(std::uint32_t& Seed);

    double RemapClamped(double Value, double InMin, double InMax, double OutMin, double OutMax);

    // Round(Value), then signed integer division by ten and multiplication by ten.
    std::int32_t RoundAndTruncateToTens(double Value);

    float PointDistanceSquared(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    // Copies the 16-byte input; X = sin(angle)*radius, Y = -cos(angle)*radius.
    inline EC_Struct::TPointF PolarToPoint(TPolarPoint Polar);

    double PointDistance(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    // Adds 360 only once for negative angles; does not fully normalize arbitrary inputs.
    double RadiansToHeadingDegrees(double Angle);

    // Subtracts 360 only once for angles above 180; does not fully normalize arbitrary inputs.
    double HeadingDegreesToRadians(double Angle);

    // Bearing from A to B: zero points upward and angles increase clockwise in screen coordinates.
    double PointBearingDegrees(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB);

    // Signed shortest turn from FromHeading to ToHeading; requires headings normalized to [0,360).
    double HeadingDifferenceDegrees(double FromHeading, double ToHeading);

    // Repeatedly adds or subtracts 360 to reach [0,360); requires a finite value small enough for Single-precision steps to change it.
    float WrapHeadingDegrees(float Angle);

    // Original unit ownership of this formatting family (aMyFunction/MessageText) is unresolved.
    // ColorTag is a complete opening tag; empty disables coloring. Replacements are
    // case-sensitive and append </color> even when the replacement text is empty.
    void ReplaceTextToken(pas::WideString& Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag);

    pas::WideString ReplaceColoredToken(pas::WideString Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag);

    pas::WideString FormatText1(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token, pas::WideString Replacement);

    // Multiple replacements run in order, including matches in text inserted earlier.
    pas::WideString FormatText2(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2);

    pas::WideString FormatText3(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2, pas::WideString Token3, pas::WideString Replacement3);

    // Returns Text unchanged when either argument is empty.
    pas::WideString WrapTextInColor(const std::u16string_view& Text, const std::u16string_view& ColorTag);

    // Normalizes the ray direction, rejects tangencies, and returns whether the selected intersection is ahead of StartPoint. No segment-length bound.
    std::uint8_t RayIntersectsOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF ThroughPoint, EC_Struct::TPointF& Intersection, float Radius);

    // Returns a decremented value, wrapping below Minimum to Maximum. Value is passed by value.
    std::int32_t DecrementWrappedValue(std::int32_t Value, std::int32_t Minimum, std::int32_t Maximum);

    // Increments Value, or resets it to Minimum when Value + 1 exceeds Maximum; returns the updated value.
    std::int32_t IncrementWrapped(std::int32_t& Value, std::int32_t Minimum, std::int32_t Maximum);

    double FractionalQuotient(std::int32_t Numerator, std::int32_t Denominator);

    std::int32_t RoundAndTruncateToFives(double Value);

    std::int32_t RoundAndTruncateToHundreds(double Value);

    // Angle is in radians, measured from the positive X axis.
    inline EC_Struct::TPointF PointFromRadiusAngle(float Radius, float Angle);

    inline EC_Struct::TPointF OffsetPointByRadiusAngle(EC_Struct::TPointF Origin, float Radius, float Angle);

    inline EC_Struct::TPointF RotateAndTranslatePoint(EC_Struct::TPointF Point, EC_Struct::TPointF Translation, float Angle);

    // Angle is ArcTan2(X,Y), measured from positive Y; squared radius uses signed 32-bit integer arithmetic.
    TPolarRadiansPoint IntegerPointToPolar(Types::TPoint Point);

    std::uint8_t HeadingDegreesToByte(double Angle);

    double ByteToHeadingDegrees(std::uint8_t Angle);

    // Normalizes finite angles to [-180,180).
    float WrapSignedHeadingDegrees(float Angle);

    std::uint8_t HeadingWithinArc(float ArcStart, float Heading, float ArcEnd);

    // Within Margin of Radius, scales Point to Radius+Margin; otherwise returns Point.
    EC_Struct::TPointF PushPointOutsideCircleBand(EC_Struct::TPointF Point, float Radius, float Margin);

    EC_Struct::TPointF RotatePointQuarterTurn(EC_Struct::TPointF Center, EC_Struct::TPointF Point);

    std::uint8_t IntersectLines(EC_Struct::TPointF A1, EC_Struct::TPointF A2, EC_Struct::TPointF B1, EC_Struct::TPointF B2, EC_Struct::TPointF& Intersection);

    // Tests top, bottom, left, then right; returns the first edge hit, not the nearest. Corners must be ordered.
    std::uint8_t SegmentIntersectsRectEdges(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF TopLeft, EC_Struct::TPointF BottomRight, EC_Struct::TPointF& Intersection);

    // Accepts a start inside the circle; rejects tangencies.
    std::uint8_t SegmentIntersectsCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF Center, float Radius);

    // Requires both endpoints outside and segment length at least the start's distance from the origin.
    std::uint8_t SegmentCrossesOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, float Radius);

    // Returns sin(Angle) times the radius of the circle through the endpoints tangent to Heading at StartPoint; angles are degrees.
    double CalculateTangentArcOffset(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, double Heading, double Angle);

    void CircleTangentPoints(EC_Struct::TPointF Point, float Radius, EC_Struct::TPointF& LeftPoint, EC_Struct::TPointF& RightPoint);

    // Seed selects a heading offset in [90,269] degrees without advancing.
    EC_Struct::TPointF PointBehindHeading(EC_Struct::TPointF Origin, double Heading, double Distance, std::uint32_t Seed);

    std::int32_t IntegerPointDistancePlusOne(Types::TPoint PointA, Types::TPoint PointB);

    EC_Struct::TPointF MakeFloatPoint(std::int32_t X, std::int32_t Y);

    pas::WideString NormalizeTextHighlightColors(pas::WideString Text);

    void TObjectList_Destroy(TObjectList* Self);

} // namespace aMyFunction

#include "inline/aMyFunction.hpp"
