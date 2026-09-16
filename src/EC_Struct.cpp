#include "layout/EC_Struct.hpp"
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"
#include "units/System.hpp"

namespace EC_Struct {
    // Freed on both startup shutdown paths; concrete class unresolved.
    pas::Object* StartupCleanupObject{};

    TVector3D MakeVector3D(double X, double Y, double Z) {
        TVector3D Result{};
        Result.X = X;
        Result.Y = Y;
        Result.Z = Z;
        return Result;
    }

    TPointF MakePointF(float X, float Y) {
        TPointF Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

    // Truncates each coordinate toward zero.
    Types::TPoint TruncatePointF(TPointF Point) {
        Types::TPoint Result{};
        Result.X = System::Trunc(Point.X);
        Result.Y = System::Trunc(Point.Y);
        return Result;
    }

    Types::TPoint RoundPointF(TPointF Point) {
        Types::TPoint Result{};
        Result.X = System::Round(Point.X);
        Result.Y = System::Round(Point.Y);
        return Result;
    }

    TPointF PointToPointF(Types::TPoint Point) {
        TPointF Result{};
        Result.X = Point.X;
        Result.Y = Point.Y;
        return Result;
    }

    // Integer division rounds toward zero.
    Types::TPoint HalfPoint(Types::TPoint Point) {
        Types::TPoint Result{};
        Result.X = Point.X / 2;
        Result.Y = Point.Y / 2;
        return Result;
    }

    Types::TPoint AddPoints(Types::TPoint Left, Types::TPoint Right) {
        Types::TPoint Result{};
        Result.X = Left.X + Right.X;
        Result.Y = Left.Y + Right.Y;
        return Result;
    }

    Types::TPoint SubtractPoints(Types::TPoint Left, Types::TPoint Right) {
        Types::TPoint Result{};
        Result.X = Left.X - Right.X;
        Result.Y = Left.Y - Right.Y;
        return Result;
    }

    TPointF HalfPointF(TPointF Point) {
        TPointF Result{};
        Result.X = pas::real_divide(Point.X, 2.0L);
        Result.Y = pas::real_divide(Point.Y, 2.0L);
        return Result;
    }

    TPointF AddPointsF(TPointF Left, TPointF Right) {
        TPointF Result{};
        Result.X = static_cast<long double>(Left.X) + Right.X;
        Result.Y = static_cast<long double>(Left.Y) + Right.Y;
        return Result;
    }

    TPointF SubtractPointsF(TPointF Left, TPointF Right) {
        TPointF Result{};
        Result.X = static_cast<long double>(Left.X) - Right.X;
        Result.Y = static_cast<long double>(Left.Y) - Right.Y;
        return Result;
    }

    void TObjectEx_Create(TObjectEx* Self) {
        pas::object_create(Self);
    }

    void TObjectEx_Destroy(TObjectEx* Self) {
        pas::object_destroy(Self);
    }

    void TObjectEx::p_destroy() {
        EC_Struct::TObjectEx_Destroy(this);
    }

} // namespace EC_Struct
