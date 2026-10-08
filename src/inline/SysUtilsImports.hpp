#pragma once
#include "units/SysUtilsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace SysUtilsImports {
    inline std::int32_t AnsiStrComp(std::uint8_t* Left, std::uint8_t* Right) {
        return WindowsSdk::CompareStringA(WindowsSdk::LOCALE_USER_DEFAULT, 0u, Left, -1, Right, -1) - 2;
    }

    inline pas::AnsiString FixedFloatText(double Value, std::int32_t Precision, std::int32_t Digits,
        std::uint8_t Separator) {
        return pas::float_to_fixed(Value, Precision, Digits, Separator);
    }

} // namespace SysUtilsImports
