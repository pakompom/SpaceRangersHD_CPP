#pragma once
#include "types/fCfgSettings.hpp"

namespace fCfgSettings {
    static_assert(sizeof(void*) != 4 || sizeof(fCfgSettings::TfCfgSettings) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, ActiveGroupIndex) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, BuildGroupIndex) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, CurrentOptionName) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, GroupPanels) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, GroupNextY) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, SettingsMode) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, ModeButtonState) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, ModeLeftPosition) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, ModeRightPosition) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, GroupButtonTops) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, ModeLeaveTimer) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, RobotAvailability) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(fCfgSettings::TfCfgSettings, HasInstalledPackages) == 316);

} // namespace fCfgSettings
