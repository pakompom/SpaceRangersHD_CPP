#pragma once
#include "types/fPanelRuins.hpp"

namespace fPanelRuins {
    static_assert(sizeof(void*) != 4 || sizeof(fPanelRuins::TfPanelRuins) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelRuins::TfPanelRuins, Screen) == 4);

} // namespace fPanelRuins
