#pragma once
#include "types/MessageText.hpp"

namespace MessageText {
    extern MessageText::TQuestMessages* QuestMessages;

    void TQuestMessages_Create(TQuestMessages* Self);

    void TQuestMessages_Destroy(TQuestMessages* Self);

} // namespace MessageText
