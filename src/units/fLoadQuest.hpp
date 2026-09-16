#pragma once
#include "types/fLoadQuest.hpp"

namespace fLoadQuest {
    extern std::uint32_t QuestNameColor;

    void TfLoadQuest_Create(TfLoadQuest* Self);

    void TfLoadQuest_Destroy(TfLoadQuest* Self);

} // namespace fLoadQuest
