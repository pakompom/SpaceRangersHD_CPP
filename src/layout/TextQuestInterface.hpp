#pragma once
#include "types/TextQuestInterface.hpp"

namespace TextQuestInterface {
    static_assert(sizeof(void*) != 4 || sizeof(TextQuestInterface::TTextQuestInterface) == 4);

} // namespace TextQuestInterface
