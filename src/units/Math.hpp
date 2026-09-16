#pragma once
#include "runtime_support.hpp"

namespace Math {
    inline pas::Extended IntPower(pas::Extended Base, std::int32_t Exponent);

    inline pas::Extended ArcTan2(pas::Extended Y, pas::Extended X);

    inline pas::Extended Power(pas::Extended Base, pas::Extended Exponent);

} // namespace Math

#include "inline/Math.hpp"
