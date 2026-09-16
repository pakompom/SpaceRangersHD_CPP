#include "types/Types.hpp"
#include "units/ClassesImports.hpp"

namespace ClassesImports {
    Types::TPoint Point(std::int32_t X, std::int32_t Y) {
        Types::TPoint Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

    Types::TRect Rect(std::int32_t Left, std::int32_t Top, std::int32_t Right, std::int32_t Bottom) {
        Types::TRect Result{};
        Result.Left = Left;
        Result.Top = Top;
        Result.Right = Right;
        Result.Bottom = Bottom;
        return Result;
    }

} // namespace ClassesImports
