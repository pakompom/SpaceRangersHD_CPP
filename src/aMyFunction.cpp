#include "layout/aMyFunction.hpp"
#include "types/Classes.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "units/EC_Str.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"

namespace aMyFunction {
    pas::WideString InfoNameColorTag = u"<color=57,239,255>"_w;

    pas::WideString InfoHullSeriesColorTag = u"<color=82,166,255>"_w;

    const float PolarDegreesToRadians = 0.017453292f;

    std::int32_t RandomIntRange(std::int32_t BoundA, std::int32_t BoundB) {
        if (BoundA <= BoundB) {
            return pas::random(BoundB - BoundA + 1, &System::RandSeed) + BoundA;
        }
        return pas::random(BoundA - BoundB + 1, &System::RandSeed) + BoundB;
    }

    std::int32_t SeededRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t Seed) {
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->IsChaoticRandomEnabled()) {
            if (BoundA <= BoundB) {
                return pas::random(BoundB - BoundA + 1, &System::RandSeed) + BoundA;
            }
            return pas::random(BoundA - BoundB + 1, &System::RandSeed) + BoundB;
        } else if (BoundA < BoundB) {
            return pas::imod(Seed, static_cast<std::uint32_t>(BoundB - BoundA + 1)) + BoundA;
        } else {
            return pas::imod(Seed, static_cast<std::uint32_t>(BoundA - BoundB + 1)) + BoundB;
        }
    }

    float RandomUnitFloat() {
        return pas::real_divide(aMyFunction::RandomIntRange(1, 1000), 1.0E+3L);
    }

    float SeededRandomUnitFloat(std::uint32_t Seed) {
        return pas::real_divide(aMyFunction::SeededRandomIntRange(1, 1000, Seed), 1.0E+3L);
    }

    double RandomFloatRange(double BoundA, double BoundB) {
        std::int32_t trunc = System::Trunc(BoundB * 1.0E+3L + 1.0L);
        std::int32_t trunc_2 = System::Trunc(BoundA * 1.0E+3L + 1.0L);
        return pas::real_divide(aMyFunction::RandomIntRange(trunc_2, trunc), 1.0E+3L);
    }

    double SeededRandomFloatRange(std::uint32_t Seed, double BoundA, double BoundB) {
        std::int32_t trunc = System::Trunc(BoundB * 1.0E+3L + 1.0L);
        std::int32_t trunc_2 = System::Trunc(BoundA * 1.0E+3L + 1.0L);
        return pas::real_divide(aMyFunction::SeededRandomIntRange(trunc_2, trunc, Seed), 1.0E+3L);
    }

    std::uint32_t StepRandomSeed(std::uint32_t Seed) {
        return Seed * 7981 + 567;
    }

    std::uint32_t AdvanceRandomSeed(std::uint32_t& Seed) {
        std::uint32_t OldSeed = Seed;
        Seed = Seed * 7981 + 567 + Seed / 7981;
        if (Seed == OldSeed) {
            Seed = Seed * 7281 + 517 + Seed / 7181;
        }
        return Seed;
    }

    std::int32_t NextRandomIntRange(std::int32_t BoundA, std::int32_t BoundB, std::uint32_t& Seed) {
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->IsChaoticRandomEnabled()) {
            return aMyFunction::RandomIntRange(BoundA, BoundB);
        }
        std::uint32_t OldSeed = Seed;
        Seed = Seed * 7981 + 567 + Seed / 7981;
        if (Seed == OldSeed) {
            Seed = Seed * 7281 + 517 + Seed / 7181;
        }
        if (BoundA < BoundB) {
            return pas::imod(Seed, static_cast<std::uint32_t>(BoundB - BoundA + 1)) + BoundA;
        }
        return pas::imod(Seed, static_cast<std::uint32_t>(BoundA - BoundB + 1)) + BoundB;
    }

    double NextRandomFloatRange(double BoundA, double BoundB, std::uint32_t& Seed) {
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->IsChaoticRandomEnabled()) {
            return aMyFunction::RandomFloatRange(BoundA, BoundB);
        }
        std::uint32_t OldSeed = Seed;
        Seed = Seed * 7981 + 567 + Seed / 7931;
        if (Seed == OldSeed) {
            Seed = Seed * 6281 + 317 + Seed / 7311;
        }
        std::int32_t trunc = System::Trunc(BoundB * 1.0E+3L + 1.0L);
        std::int32_t trunc_2 = System::Trunc(BoundA * 1.0E+3L + 1.0L);
        return pas::real_divide(aMyFunction::SeededRandomIntRange(trunc_2, trunc, Seed), 1.0E+3L);
    }

    double NextRandomUnitFloat(std::uint32_t& Seed) {
        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->IsChaoticRandomEnabled()) {
            return aMyFunction::RandomFloatRange(0.0, 1.0);
        }
        std::uint32_t OldSeed = Seed;
        Seed = Seed * 7981 + 5671;
        if (Seed == OldSeed) {
            Seed = Seed * 5331 + 3417;
        }
        return System::Frac(pas::real_divide(Seed, 10011001.0L));
    }

    double RemapClamped(double Value, double InMin, double InMax, double OutMin, double OutMax) {
        if (!(Value > InMin)) {
            return OutMin;
        } else if (!(Value < InMax)) {
            return OutMax;
        } else {
            return pas::real_divide(static_cast<long double>(Value) - InMin, static_cast<long double>(InMax) - InMin) * (static_cast<long double>(OutMax) - OutMin) + OutMin;
        }
    }

    std::int32_t RoundAndTruncateToTens(double Value) {
        return System::Round(Value) / 10 * 10;
    }

    float PointDistanceSquared(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB) {
        float X = static_cast<long double>(PointA.X) - PointB.X;
        float Y = static_cast<long double>(PointA.Y) - PointB.Y;
        return static_cast<long double>(X) * X + static_cast<long double>(Y) * Y;
    }

    double PointDistance(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB) {
        float X = static_cast<long double>(PointA.X) - PointB.X;
        float Y = static_cast<long double>(PointA.Y) - PointB.Y;
        return System::Sqrt(static_cast<long double>(X) * X + static_cast<long double>(Y) * Y);
    }

    double RadiansToHeadingDegrees(double Angle) {
        double Result = Angle * 57.29578049044296832L;
        if (Result < 0.0L) {
            return 3.6E+2L + Result;
        }
        return Result;
    }

    double HeadingDegreesToRadians(double Angle) {
        if (Angle > 1.8E+2L) {
            Angle = Angle - 3.6E+2L;
        }
        return Angle * 0.017453292222222222223L;
    }

    double PointBearingDegrees(EC_Struct::TPointF PointA, EC_Struct::TPointF PointB) {
        return aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(static_cast<long double>(PointB.X) - PointA.X, -(static_cast<long double>(PointB.Y) - PointA.Y)));
    }

    double HeadingDifferenceDegrees(double FromHeading, double ToHeading) {
        double Result = static_cast<long double>(ToHeading) - FromHeading;
        if (FromHeading < 1.8E+2L) {
            if (Result > 1.8E+2L) {
                return Result - 3.6E+2L;
            }
            return Result;
        } else if (Result < -1.8E+2L) {
            return 3.6E+2L + Result;
        } else {
            return Result;
        }
    }

    float WrapHeadingDegrees(float Angle) {
        while (Angle >= 3.6E+2L) {
            Angle = Angle - 3.6E+2L;
        }
        while (Angle < 0.0L) {
            Angle = 3.6E+2L + Angle;
        }
        return Angle;
    }

    void ReplaceTextToken(pas::WideString& Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag) {
        if (ColorTag != u"") {
            Replacement = pas::concat_wide({ColorTag, Replacement, u"</color>"});
        }
        Text = EC_Str::ReplaceAllWideString(Text, Token, Replacement);
    }

    pas::WideString ReplaceColoredToken(pas::WideString Text, pas::WideString Token, pas::WideString Replacement, pas::WideString ColorTag) {
        if (ColorTag != u"") {
            Replacement = pas::concat_wide({ColorTag, Replacement, u"</color>"});
        }
        return EC_Str::ReplaceAllWideString(Text, Token, Replacement);
    }

    pas::WideString FormatText1(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token, pas::WideString Replacement) {
        if (ColorTag != u"") {
            Replacement = pas::concat_wide({ColorTag, Replacement, u"</color>"});
        }
        return EC_Str::ReplaceAllWideString(Text, Token, Replacement);
    }

    pas::WideString FormatText2(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2) {
        if (ColorTag != u"") {
            Replacement1 = pas::concat_wide({ColorTag, Replacement1, u"</color>"});
            Replacement2 = pas::concat_wide({ColorTag, Replacement2, u"</color>"});
        }
        return EC_Str::ReplaceAllWideString(EC_Str::ReplaceAllWideString(Text, Token1, Replacement1), Token2, Replacement2);
    }

    pas::WideString FormatText3(pas::WideString Text, pas::WideString ColorTag, pas::WideString Token1, pas::WideString Replacement1, pas::WideString Token2, pas::WideString Replacement2, pas::WideString Token3, pas::WideString Replacement3) {
        if (ColorTag != u"") {
            Replacement1 = pas::concat_wide({ColorTag, Replacement1, u"</color>"});
            Replacement2 = pas::concat_wide({ColorTag, Replacement2, u"</color>"});
            Replacement3 = pas::concat_wide({ColorTag, Replacement3, u"</color>"});
        }
        return EC_Str::ReplaceAllWideString(EC_Str::ReplaceAllWideString(EC_Str::ReplaceAllWideString(Text, Token1, Replacement1), Token2, Replacement2), Token3, Replacement3);
    }

    pas::WideString WrapTextInColor(pas::WideString Text, pas::WideString ColorTag) {
        if (ColorTag != u"" && Text != u"") {
            return pas::concat_wide({ColorTag, Text, u"</color>"});
        }
        return Text;
    }

    std::uint8_t RayIntersectsOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF ThroughPoint, EC_Struct::TPointF& Intersection, float Radius) {
        float T2{};
        float DX = static_cast<long double>(ThroughPoint.X) - StartPoint.X;
        float DY = static_cast<long double>(ThroughPoint.Y) - StartPoint.Y;
        float T1 = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(DX) * DX + static_cast<long double>(DY) * DY));
        DX = static_cast<long double>(DX) * T1;
        DY = static_cast<long double>(DY) * T1;
        float CX = -StartPoint.X;
        float CY = -StartPoint.Y;
        float CenterDistanceSquared = static_cast<long double>(CX) * CX + static_cast<long double>(CY) * CY;
        float Projection = static_cast<long double>(CX) * DX + static_cast<long double>(CY) * DY;
        float Discriminant = pas::sqr(static_cast<pas::Extended>(Radius)) - CenterDistanceSquared + static_cast<long double>(Projection) * Projection;
        if (Discriminant <= 0.0L) {
            return false;
        }
        Discriminant = System::Sqrt(Discriminant);
        if (Projection < Discriminant) {
            T1 = static_cast<long double>(Projection) + Discriminant;
            T2 = static_cast<long double>(Projection) - Discriminant;
        } else {
            T1 = static_cast<long double>(Projection) - Discriminant;
            T2 = static_cast<long double>(Projection) + Discriminant;
        }
        if (std::fabs(static_cast<pas::Extended>(T1)) < 0.001L) {
            T1 = T2;
        }
        Intersection.X = static_cast<long double>(DX) * T1 + StartPoint.X;
        Intersection.Y = static_cast<long double>(DY) * T1 + StartPoint.Y;
        return T1 > 0.001L;
    }

    std::int32_t DecrementWrappedValue(std::int32_t Value, std::int32_t Minimum, std::int32_t Maximum) {
        if (Value - 1 < Minimum) {
            Value = Maximum;
        } else {
            --Value;
        }
        return Value;
    }

    std::int32_t IncrementWrapped(std::int32_t& Value, std::int32_t Minimum, std::int32_t Maximum) {
        if (Value + 1 > Maximum) {
            Value = Minimum;
        } else {
            ++Value;
        }
        return Value;
    }

    double FractionalQuotient(std::int32_t Numerator, std::int32_t Denominator) {
        return System::Frac(pas::real_divide(Numerator, Denominator));
    }

    std::int32_t RoundAndTruncateToFives(double Value) {
        return System::Round(Value) / 5 * 5;
    }

    std::int32_t RoundAndTruncateToHundreds(double Value) {
        return System::Round(Value) / 100 * 100;
    }

    TPolarRadiansPoint IntegerPointToPolar(Types::TPoint Point) {
        TPolarRadiansPoint Result{};
        Result.Radius = System::Sqrt(Point.X * Point.X + Point.Y * Point.Y);
        Result.AngleRadians = Math::ArcTan2(Point.X, Point.Y);
        return Result;
    }

    std::uint8_t HeadingDegreesToByte(double Angle) {
        return System::Round(pas::real_divide(Angle * 256.0L, 3.6E+2L));
    }

    double ByteToHeadingDegrees(std::uint8_t Angle) {
        return Angle * 1.40625L;
    }

    float WrapSignedHeadingDegrees(float Angle) {
        while (Angle >= 1.8E+2L) {
            Angle = Angle - 3.6E+2L;
        }
        while (Angle < -1.8E+2L) {
            Angle = 3.6E+2L + Angle;
        }
        return Angle;
    }

    std::uint8_t HeadingWithinArc(float ArcStart, float Heading, float ArcEnd) {
        float A = aMyFunction::HeadingDifferenceDegrees(ArcStart, Heading);
        float B = aMyFunction::HeadingDifferenceDegrees(ArcStart, ArcEnd);
        if (A < 0.0L && B > 0.0L || A > 0.0L && B < 0.0L) {
            return false;
        }
        A = aMyFunction::HeadingDifferenceDegrees(ArcEnd, Heading);
        B = aMyFunction::HeadingDifferenceDegrees(ArcEnd, ArcStart);
        if (A < 0.0L && B > 0.0L || A > 0.0L && B < 0.0L) {
            return false;
        }
        return true;
    }

    EC_Struct::TPointF PushPointOutsideCircleBand(EC_Struct::TPointF Point, float Radius, float Margin) {
        EC_Struct::TPointF Result{};
        float Distance = System::Sqrt(static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y);
        if (std::fabs(static_cast<long double>(Radius) - Distance) <= Margin) {
            Result.X = pas::real_divide(Point.X, Distance) * (static_cast<long double>(Radius) + Margin);
            Result.Y = pas::real_divide(Point.Y, Distance) * (static_cast<long double>(Radius) + Margin);
        } else {
            Result = Point;
        }
        return Result;
    }

    EC_Struct::TPointF RotatePointQuarterTurn(EC_Struct::TPointF Center, EC_Struct::TPointF Point) {
        EC_Struct::TPointF Result{};
        Result.X = Center.X - (static_cast<long double>(Point.Y) - Center.Y);
        Result.Y = static_cast<long double>(Point.X) - Center.X + Center.Y;
        return Result;
    }

    std::uint8_t IntersectLines(EC_Struct::TPointF A1, EC_Struct::TPointF A2, EC_Struct::TPointF B1, EC_Struct::TPointF B2, EC_Struct::TPointF& Intersection) {
        double AX = static_cast<long double>(A2.X) - A1.X;
        double AY = static_cast<long double>(A2.Y) - A1.Y;
        double BX = static_cast<long double>(B2.X) - B1.X;
        double BY = static_cast<long double>(B2.Y) - B1.Y;
        double Divisor = static_cast<long double>(AY) * BX - static_cast<long double>(BY) * AX;
        if (Divisor == 0.0L) {
            return false;
        }
        Intersection.X = pas::real_divide((static_cast<long double>(B1.Y) - A1.Y) * AX * BX + static_cast<long double>(AY) * BX * A1.X - static_cast<long double>(BY) * AX * B1.X, Divisor);
        if (AX != 0.0L) {
            Intersection.Y = pas::real_divide((static_cast<long double>(Intersection.X) - A1.X) * AY, AX) + A1.Y;
        } else {
            Intersection.Y = pas::real_divide((static_cast<long double>(Intersection.X) - B1.X) * BY, BX) + B1.Y;
        }
        return true;
    }

    std::uint8_t SegmentIntersectsRectEdges(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF TopLeft, EC_Struct::TPointF BottomRight, EC_Struct::TPointF& Intersection) {
        EC_Struct::TPointF A{};
        EC_Struct::TPointF B{};
        A.X = TopLeft.X;
        A.Y = TopLeft.Y;
        B.X = BottomRight.X;
        B.Y = TopLeft.Y;
        if (aMyFunction::IntersectLines(StartPoint, EndPoint, A, B, Intersection)) {
            if (Intersection.X >= A.X && Intersection.X <= B.X && Intersection.Y >= pas::real_min<float>(StartPoint.Y, EndPoint.Y) && Intersection.Y <= pas::real_max<float>(StartPoint.Y, EndPoint.Y)) {
                return true;
            }
        }
        A.X = TopLeft.X;
        A.Y = BottomRight.Y;
        B.X = BottomRight.X;
        B.Y = BottomRight.Y;
        if (aMyFunction::IntersectLines(StartPoint, EndPoint, A, B, Intersection)) {
            if (Intersection.X >= A.X && Intersection.X <= B.X && Intersection.Y >= pas::real_min<float>(StartPoint.Y, EndPoint.Y) && Intersection.Y <= pas::real_max<float>(StartPoint.Y, EndPoint.Y)) {
                return true;
            }
        }
        A.X = TopLeft.X;
        A.Y = TopLeft.Y;
        B.X = TopLeft.X;
        B.Y = BottomRight.Y;
        if (aMyFunction::IntersectLines(StartPoint, EndPoint, A, B, Intersection)) {
            if (Intersection.Y >= A.Y && Intersection.Y <= B.Y && Intersection.X >= pas::real_min<float>(StartPoint.X, EndPoint.X) && Intersection.X <= pas::real_max<float>(StartPoint.X, EndPoint.X)) {
                return true;
            }
        }
        A.X = BottomRight.X;
        A.Y = TopLeft.Y;
        B.X = BottomRight.X;
        B.Y = BottomRight.Y;
        if (aMyFunction::IntersectLines(StartPoint, EndPoint, A, B, Intersection)) {
            if (Intersection.Y >= A.Y && Intersection.Y <= B.Y && Intersection.X >= pas::real_min<float>(StartPoint.X, EndPoint.X) && Intersection.X <= pas::real_max<float>(StartPoint.X, EndPoint.X)) {
                return true;
            }
        }
        return false;
    }

    std::uint8_t SegmentIntersectsCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, EC_Struct::TPointF Center, float Radius) {
        float T2{};
        if (pas::sqr(static_cast<long double>(StartPoint.X) - Center.X) + pas::sqr(static_cast<long double>(StartPoint.Y) - Center.Y) < static_cast<long double>(Radius) * Radius) {
            return true;
        }
        float DX = static_cast<long double>(EndPoint.X) - StartPoint.X;
        float DY = static_cast<long double>(EndPoint.Y) - StartPoint.Y;
        float T1 = System::Sqrt(static_cast<long double>(DX) * DX + static_cast<long double>(DY) * DY);
        if (T1 == 0.0L) {
            return false;
        }
        T1 = pas::real_divide(1.0L, T1);
        DX = static_cast<long double>(DX) * T1;
        DY = static_cast<long double>(DY) * T1;
        float CX = static_cast<long double>(Center.X) - StartPoint.X;
        float CY = static_cast<long double>(Center.Y) - StartPoint.Y;
        float CenterDistanceSquared = static_cast<long double>(CX) * CX + static_cast<long double>(CY) * CY;
        float Projection = static_cast<long double>(CX) * DX + static_cast<long double>(CY) * DY;
        float Discriminant = pas::sqr(static_cast<pas::Extended>(Radius)) - CenterDistanceSquared + static_cast<long double>(Projection) * Projection;
        if (Discriminant <= 0.0L) {
            return false;
        }
        Discriminant = System::Sqrt(Discriminant);
        if (Projection < Discriminant) {
            T1 = static_cast<long double>(Projection) + Discriminant;
            T2 = static_cast<long double>(Projection) - Discriminant;
        } else {
            T1 = static_cast<long double>(Projection) - Discriminant;
            T2 = static_cast<long double>(Projection) + Discriminant;
        }
        float SegmentLength = System::Sqrt(pas::sqr(static_cast<long double>(StartPoint.X) - EndPoint.X) + pas::sqr(static_cast<long double>(StartPoint.Y) - EndPoint.Y));
        return T1 >= 0.0L && T1 <= SegmentLength || T2 >= 0.0L && T2 <= SegmentLength;
    }

    std::uint8_t SegmentCrossesOriginCircle(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, float Radius) {
        EC_Struct::TPointF Delta{};
        std::uint8_t Result = false;
        float RadiusSquared = static_cast<long double>(Radius) * Radius;
        float StartDistanceSquared = static_cast<long double>(StartPoint.X) * StartPoint.X + static_cast<long double>(StartPoint.Y) * StartPoint.Y;
        if (StartDistanceSquared < RadiusSquared) {
            return Result;
        }
        if (static_cast<long double>(EndPoint.X) * EndPoint.X + static_cast<long double>(EndPoint.Y) * EndPoint.Y < RadiusSquared) {
            return Result;
        }
        Delta.X = static_cast<long double>(EndPoint.X) - StartPoint.X;
        Delta.Y = static_cast<long double>(EndPoint.Y) - StartPoint.Y;
        float LengthSquared = static_cast<long double>(Delta.X) * Delta.X + static_cast<long double>(Delta.Y) * Delta.Y;
        if (LengthSquared < StartDistanceSquared) {
            return Result;
        }
        float Projection = pas::real_divide(static_cast<long double>(-StartPoint.X) * Delta.X - static_cast<long double>(StartPoint.Y) * Delta.Y, System::Sqrt(LengthSquared));
        if (Projection < 0.0L) {
            return false;
        }
        return StartDistanceSquared - static_cast<long double>(Projection) * Projection < static_cast<long double>(Radius) * Radius;
    }

    double CalculateTangentArcOffset(EC_Struct::TPointF StartPoint, EC_Struct::TPointF EndPoint, double Heading, double Angle) {
        EC_Struct::TPointF Center{};
        EC_Struct::TPointF Normal{};
        EC_Struct::TPointF Midpoint{};
        double Radians = aMyFunction::HeadingDegreesToRadians(Heading);
        Normal.X = System::Sin(Radians) * 1.0E+2L + StartPoint.X;
        Normal.Y = StartPoint.Y - System::Cos(Radians) * 1.0E+2L;
        Normal = aMyFunction::RotatePointQuarterTurn(StartPoint, Normal);
        Midpoint.X = pas::real_divide(static_cast<long double>(StartPoint.X) + EndPoint.X, 2.0L);
        Midpoint.Y = pas::real_divide(static_cast<long double>(StartPoint.Y) + EndPoint.Y, 2.0L);
        if (!aMyFunction::IntersectLines(StartPoint, Normal, Midpoint, aMyFunction::RotatePointQuarterTurn(Midpoint, StartPoint), Center)) {
            return 0.0;
        }
        double Radius = aMyFunction::PointDistance(StartPoint, Center);
        double CentralAngle = 1.8E+2L - (9.0E+1L - Angle) * 2.0L;
        return System::Sin(aMyFunction::HeadingDegreesToRadians(pas::real_divide(CentralAngle, 2.0L))) * Radius;
    }

    void CircleTangentPoints(EC_Struct::TPointF Point, float Radius, EC_Struct::TPointF& LeftPoint, EC_Struct::TPointF& RightPoint) {
        float Distance = System::Sqrt(static_cast<long double>(Point.X) * Point.X + static_cast<long double>(Point.Y) * Point.Y);
        float Spread = MathImports::ArcCos(pas::real_divide(Radius, Distance));
        float Angle = Math::ArcTan2(Point.X, -Point.Y);
        LeftPoint.X = System::Sin(static_cast<long double>(Angle) + Spread) * Radius;
        LeftPoint.Y = -System::Cos(static_cast<long double>(Angle) + Spread) * Radius;
        RightPoint.X = System::Sin(static_cast<long double>(Angle) - Spread) * Radius;
        RightPoint.Y = -System::Cos(static_cast<long double>(Angle) - Spread) * Radius;
    }

    EC_Struct::TPointF PointBehindHeading(EC_Struct::TPointF Origin, double Heading, double Distance, std::uint32_t Seed) {
        EC_Struct::TPointF Result{};
        Heading = aMyFunction::HeadingDegreesToRadians(aMyFunction::WrapHeadingDegrees(Heading + 1.8E+2L + (static_cast<std::int32_t>(Seed % 180) - 90)));
        Result.X = System::Sin(Heading) * Distance + Origin.X;
        Result.Y = Origin.Y - System::Cos(Heading) * Distance;
        return Result;
    }

    std::int32_t IntegerPointDistancePlusOne(Types::TPoint PointA, Types::TPoint PointB) {
        std::int32_t X = PointA.X - PointB.X;
        std::int32_t Y = PointA.Y - PointB.Y;
        return System::Trunc(System::Sqrt(X * X + Y * Y) + 1.0L);
    }

    EC_Struct::TPointF MakeFloatPoint(std::int32_t X, std::int32_t Y) {
        EC_Struct::TPointF Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

    pas::WideString NormalizeTextHighlightColors(pas::WideString Text) {
        pas::WideString Result{};
        Result = aMyFunction::FormatText1(Text, pas::WideString(), u"<color=17,139,255>"_w, u"<color=255,240,100>"_w);
        Result = aMyFunction::FormatText1(Result, pas::WideString(), u"<color=127,127,127>"_w, u"<color=255,240,100>"_w);
        Result = aMyFunction::FormatText1(Result, pas::WideString(), u"<color=191,185,128>"_w, u"<color=255,240,100>"_w);
        Result = aMyFunction::FormatText1(Result, pas::WideString(), InfoNameColorTag, u"<color=255,240,100>"_w);
        Result = aMyFunction::FormatText1(Result, pas::WideString(), u"<color=39,172,177>"_w, u"<color=255,240,100>"_w);
        return aMyFunction::FormatText1(Result, pas::WideString(), InfoHullSeriesColorTag, u"<color=255,240,100>"_w);
    }

    void TObjectList_Destroy(TObjectList* Self) {
        Self->FreeItems();
        pas::list_destroy(Self);
    }

    void TObjectList::FreeItems() {
        std::int32_t i{};
        pas::Object* Item{};
        {
            const std::int32_t cpp_first = pas::list_count(this) - 1;
            if (cpp_first >= 0) {
                for (i = cpp_first; i >= 0; --i) {
                    if (pas::load_unaligned<void*>(pas::byte_offset(pas::list_data(this), i * sizeof(void*))) != nullptr) {
                        Item = static_cast<pas::Object*>(pas::load_unaligned<void*>(pas::byte_offset(pas::list_data(this), i * sizeof(void*))));
                        pas::list_delete(this, i);
                        pas::free(Item);
                    }
                }
            }
        }
        pas::list_clear(this);
    }

    void TObjectList::p_destroy() {
        aMyFunction::TObjectList_Destroy(this);
    }

} // namespace aMyFunction
