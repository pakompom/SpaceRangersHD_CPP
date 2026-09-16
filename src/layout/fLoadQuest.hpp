#pragma once
#include "types/fLoadQuest.hpp"

namespace fLoadQuest {
    static_assert(sizeof(void*) != 4 || sizeof(fLoadQuest::TfLoadQuestSlot) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Name) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Title) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Description) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Image) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Genre) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Length) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, QuestId) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, BackgroundImage) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, RequiredAccess) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, AlternateGroup) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuestSlot, Difficulty) == 40);
    static_assert(sizeof(void*) != 4 || sizeof(fLoadQuest::TfLoadQuest) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, LoadPanel) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, Entries) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, SelectedIndex) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, HoveredIndex) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, CompletionData) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, Category) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, AccessLevel) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadQuest::TfLoadQuest, KeyHistory) == 236);

} // namespace fLoadQuest
