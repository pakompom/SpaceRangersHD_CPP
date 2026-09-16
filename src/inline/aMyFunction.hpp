#pragma once
#include "types/EC_Struct.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

namespace aMyFunction {
    // Copies the 16-byte input; X = sin(angle)*radius, Y = -cos(angle)*radius.
    inline EC_Struct::TPointF PolarToPoint(TPolarPoint Polar) {
        EC_Struct::TPointF Result{};
        Result.X = Polar.Radius * System::Sin(static_cast<long double>(Polar.AngleDegrees) * PolarDegreesToRadians);
        Result.Y = -(Polar.Radius * System::Cos(static_cast<long double>(Polar.AngleDegrees) * PolarDegreesToRadians));
        return Result;
    }

    // Angle is in radians, measured from the positive X axis.
    inline EC_Struct::TPointF PointFromRadiusAngle(float Radius, float Angle) {
        EC_Struct::TPointF Result{};
        Result.X = Radius * System::Cos(Angle);
        Result.Y = Radius * System::Sin(Angle);
        return Result;
    }

    inline EC_Struct::TPointF OffsetPointByRadiusAngle(EC_Struct::TPointF Origin, float Radius, float Angle) {
        EC_Struct::TPointF Result{};
        Result.X = Radius * System::Cos(Angle) + Origin.X;
        Result.Y = Radius * System::Sin(Angle) + Origin.Y;
        return Result;
    }

    inline EC_Struct::TPointF RotateAndTranslatePoint(EC_Struct::TPointF Point, EC_Struct::TPointF Translation, float Angle) {
        EC_Struct::TPointF Result{};
        {
            pas::Extended cpp_left = Point.X * System::Cos(Angle);
            Result.X = cpp_left - Point.Y * System::Sin(Angle) + Translation.X;
        }
        {
            pas::Extended cpp_left_2 = Point.X * System::Sin(Angle);
            Result.Y = cpp_left_2 + Point.Y * System::Cos(Angle) + Translation.Y;
        }
        return Result;
    }

} // namespace aMyFunction
