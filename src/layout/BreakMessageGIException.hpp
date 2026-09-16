#pragma once
#include "types/BreakMessageGIException.hpp"

namespace BreakMessageGIException {
    static_assert(sizeof(void*) != 4 || sizeof(BreakMessageGIException::EBreakMessageGI) == 12);

} // namespace BreakMessageGIException
