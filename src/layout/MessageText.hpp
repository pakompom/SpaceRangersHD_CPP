#pragma once
#include "types/MessageText.hpp"

namespace MessageText {
    static_assert(sizeof(void*) != 4 || sizeof(MessageText::TQuestMessages) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(MessageText::TQuestMessages, Entries) == 4);

} // namespace MessageText
