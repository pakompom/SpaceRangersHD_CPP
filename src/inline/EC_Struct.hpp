#pragma once
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Struct {
    inline TVector3D MakeVector3D(double X, double Y, double Z) {
        TVector3D Result{};
        Result.X = X;
        Result.Y = Y;
        Result.Z = Z;
        return Result;
    }

    inline TPointF MakePointF(float X, float Y) {
        TPointF Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

    // Integer division rounds toward zero.
    inline Types::TPoint HalfPoint(Types::TPoint Point) {
        Types::TPoint Result{};
        Result.X = Point.X / 2;
        Result.Y = Point.Y / 2;
        return Result;
    }

    inline Types::TPoint AddPoints(Types::TPoint Left, Types::TPoint Right) {
        Types::TPoint Result{};
        Result.X = Left.X + Right.X;
        Result.Y = Left.Y + Right.Y;
        return Result;
    }

    inline Types::TPoint SubtractPoints(Types::TPoint Left, Types::TPoint Right) {
        Types::TPoint Result{};
        Result.X = Left.X - Right.X;
        Result.Y = Left.Y - Right.Y;
        return Result;
    }

    inline std::int32_t SquaredDistanceToPoint(const Types::TPoint& Point, std::int32_t X, std::int32_t Y) {
        return pas::sqr(Point.X - X) + pas::sqr(Point.Y - Y);
    }

    inline void TObjectEx_Create(TObjectEx* Self) {
        pas::object_create(Self);
    }

    inline void TObjectEx_Destroy(TObjectEx* Self) {
        pas::object_destroy(Self);
    }

} // namespace EC_Struct
