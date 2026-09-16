#pragma once
#include "types/BlockParException.hpp"

namespace BlockParException {
    static_assert(sizeof(void*) != 4 || sizeof(BlockParException::EBlockPar) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(BlockParException::EBlockPar, Reportable) == 12);

} // namespace BlockParException
