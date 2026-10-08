#pragma once
#include "units/Math.hpp"

namespace Math {
    inline double IntPower(double Base, std::int32_t Exponent) {
        return pas::int_power(Base, Exponent);
    }

    inline double ArcTan2(double Y, double X) {
        return pas::real_arctan2(Y, X);
    }

    inline double Power(double Base, double Exponent) {
        return pas::real_power(Base, Exponent);
    }

} // namespace Math
