#pragma once
#include "units/SysUtilsImports.hpp"

namespace SysUtilsImports {
    inline pas::AnsiString FixedFloatText(pas::Extended Value, std::int32_t Precision, std::int32_t Digits, std::uint8_t Separator) {
        return pas::float_to_fixed(Value, Precision, Digits, Separator);
    }

} // namespace SysUtilsImports
