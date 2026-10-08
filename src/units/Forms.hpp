#pragma once
#include "types/Forms.hpp"

namespace Forms {
    extern Forms::TApplication* Application;

    void UnitInitialize();

    inline void UnitFinalize();

} // namespace Forms

#include "inline/Forms.hpp"
