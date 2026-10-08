#pragma once
#include "types/MathImports.hpp"

namespace MathImports {
    double RoundTo(double AValue, TRoundToRange ADigit);

    std::int32_t Ceil(double X);

    std::int32_t Floor(double X);

    double ArcSin(double X);

    double ArcCos(double X);

    std::int32_t Sign(double AValue);

} // namespace MathImports
