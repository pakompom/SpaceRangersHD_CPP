#pragma once
#include "types/EC_Struct.hpp"
#include "units/aMyFunction.hpp"

namespace aMyFunction {
    inline std::uint32_t StepRandomSeed(std::uint32_t Seed) {
        return Seed * 7981 + 567;
    }

    inline EC_Struct::TPointF MakeFloatPoint(std::int32_t X, std::int32_t Y) {
        EC_Struct::TPointF Result{};
        Result.X = X;
        Result.Y = Y;
        return Result;
    }

} // namespace aMyFunction
