#pragma once
#include "types/fGameSettings2.hpp"

namespace fGameSettings2 {
    static_assert(sizeof(void*) != 4 || sizeof(fGameSettings2::TfGameSettings2) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, CollapsedLevelPanelTop) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, LevelPanelTop) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, LevelPanelTimer) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, PlayerRace) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, CharacterPreset) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, CaptainPortraitIndex) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, LastPortraitByRace) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, StartingSkills) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, SelectedSkillSlot) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, StartingItemChoices) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, SelectedItemSlot) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, ItemTypeByChoice) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, DifficultyPreset) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, DifficultyLevels) == 285);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, PlayerNameEdited) == 293);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, PlayerNameValid) == 294);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, IronWillImage) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, IronWillLabel) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, IronWill) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, ActiveExtendedGroup) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, ExtendedGroupPanels) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, ExtendedGroupNextY) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, BuildExtendedGroup) == 344);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, CurrentExtendedOption) == 348);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings2::TfGameSettings2, ExtendedGroupButtonTops) == 352);

} // namespace fGameSettings2
