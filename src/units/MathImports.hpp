#pragma once
#include "types/MathImports.hpp"

namespace MathImports {
    double RoundTo(double AValue, TRoundToRange ADigit);

    std::int32_t Ceil(pas::Extended X);

    std::int32_t Floor(pas::Extended X);

    pas::Extended ArcSin(pas::Extended X);

    pas::Extended ArcCos(pas::Extended X);

    std::int32_t Sign(double AValue);

} // namespace MathImports
