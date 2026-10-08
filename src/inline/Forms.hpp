#pragma once
#include "units/Forms.hpp"

namespace Forms {
    inline void UnitFinalize() {
        pas::free(Application);
    }

} // namespace Forms
