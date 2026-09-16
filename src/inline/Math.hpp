#pragma once
#include "units/Math.hpp"

namespace Math {
    inline pas::Extended IntPower(pas::Extended Base, std::int32_t Exponent) {
        return pas::int_power(Base, Exponent);
    }

    inline pas::Extended ArcTan2(pas::Extended Y, pas::Extended X) {
        return pas::real_arctan2(Y, X);
    }

    inline pas::Extended Power(pas::Extended Base, pas::Extended Exponent) {
        return pas::real_power(Base, Exponent);
    }

} // namespace Math
