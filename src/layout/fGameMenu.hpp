#pragma once
#include "types/fGameMenu.hpp"

namespace fGameMenu {
    static_assert(sizeof(void*) != 4 || sizeof(fGameMenu::TfGameMenu) == 208);

} // namespace fGameMenu
