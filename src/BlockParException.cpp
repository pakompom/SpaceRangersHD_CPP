#include "layout/BlockParException.hpp"
#include "units/BlockParException.hpp"

namespace BlockParException {
    void EBlockPar_Create(EBlockPar* Self, pas::AnsiString Message, std::uint8_t AReportable) {
        pas::exception_create(Self, Message);
        Self->Reportable = AReportable;
    }

    // False suppresses the reporting flag in ExceptionInfo.
    std::uint8_t EBlockPar::IsReportable() {
        return Reportable;
    }

} // namespace BlockParException
