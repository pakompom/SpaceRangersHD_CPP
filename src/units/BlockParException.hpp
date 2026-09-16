#pragma once
#include "types/BlockParException.hpp"

namespace BlockParException {
    void EBlockPar_Create(EBlockPar* Self, pas::AnsiString Message, std::uint8_t AReportable);

} // namespace BlockParException
