#include "layout/EC_Struct.hpp"
#include "types/Types.hpp"
#include "units/EC_Struct.hpp"

namespace EC_Struct {
    // Freed on both startup shutdown paths; concrete class unresolved.
    pas::Object* StartupCleanupObject{};

    // Truncates each coordinate toward zero.
    Types::TPoint TruncatePointF(TPointF Point) {
        Types::TPoint Result{};
        Result.X = pas::real_trunc(Point.X);
        Result.Y = pas::real_trunc(Point.Y);
        return Result;
    }

    Types::TPoint RoundPointF(TPointF Point) {
        Types::TPoint Result{};
        Result.X = pas::real_round(Point.X);
        Result.Y = pas::real_round(Point.Y);
        return Result;
    }

    TPointF PointToPointF(Types::TPoint Point) {
        TPointF Result{};
        Result.X = Point.X;
        Result.Y = Point.Y;
        return Result;
    }

    TPointF HalfPointF(TPointF Point) {
        TPointF Result{};
        Result.X = static_cast<float>(Point.X / 2.0);
        Result.Y = static_cast<float>(Point.Y / 2.0);
        return Result;
    }

    TPointF AddPointsF(TPointF Left, TPointF Right) {
        TPointF Result{};
        Result.X = static_cast<float>(static_cast<double>(Left.X) + Right.X);
        Result.Y = static_cast<float>(static_cast<double>(Left.Y) + Right.Y);
        return Result;
    }

    TPointF SubtractPointsF(TPointF Left, TPointF Right) {
        TPointF Result{};
        Result.X = static_cast<float>(static_cast<double>(Left.X) - Right.X);
        Result.Y = static_cast<float>(static_cast<double>(Left.Y) - Right.Y);
        return Result;
    }

    // Returns false without writing Intersection when the rectangles do not overlap.
    std::uint8_t IntersectRects(Types::TRect& Intersection, const Types::TRect& First, const Types::TRect& Second) {
        // The original kernel tests all edges before writing, then stores Left,
        // Right, Top, Bottom in that order. Inputs may alias the destination.
        std::uint8_t Result = false;
        if (Second.Left >= First.Right || Second.Right <= First.Left || Second.Top >= First.Bottom || Second.Bottom <=
                First.Top) {
            return Result;
        }
        if (Second.Left >= First.Left) {
            Intersection.Left = Second.Left;
        } else {
            Intersection.Left = First.Left;
        }
        if (Second.Right <= First.Right) {
            Intersection.Right = Second.Right;
        } else {
            Intersection.Right = First.Right;
        }
        if (Second.Top >= First.Top) {
            Intersection.Top = Second.Top;
        } else {
            Intersection.Top = First.Top;
        }
        if (Second.Bottom <= First.Bottom) {
            Intersection.Bottom = Second.Bottom;
        } else {
            Intersection.Bottom = First.Bottom;
        }
        return true;
    }

    void TObjectEx::p_destroy() {
        EC_Struct::TObjectEx_Destroy(this);
    }

} // namespace EC_Struct
