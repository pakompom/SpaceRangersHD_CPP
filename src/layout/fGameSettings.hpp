#pragma once
#include "types/fGameSettings.hpp"

namespace fGameSettings {
    static_assert(sizeof(void*) != 4 || sizeof(fGameSettings::TThreadCreateNewGame) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, PlayerRace) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, DifficultyLevels) == 45);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, CaptainPortraitIndex) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, PlayerName) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, CharacterPreset) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, StartingItemTypes) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, StartingSkills) == 70);
    static_assert(sizeof(void*) != 4 || offsetof(fGameSettings::TThreadCreateNewGame, IronWill) == 72);

} // namespace fGameSettings
