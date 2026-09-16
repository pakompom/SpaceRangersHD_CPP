#pragma once
#include "types/fLoadRobot.hpp"

namespace fLoadRobot {
    static_assert(sizeof(void*) != 4 || sizeof(fLoadRobot::TfLoadRobotSlot) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, Name) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, FileName) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, MapIndex) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, Image) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, Access) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, Side) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, AlternateBackground) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobotSlot, Length) == 28);
    static_assert(sizeof(void*) != 4 || sizeof(fLoadRobot::TfLoadRobot) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, LoadPanel) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, Entries) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, SelectedIndex) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, HoveredIndex) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, BattleResult) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, CompletionData) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, Category) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, UnlockedAccess) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, KeyHistory) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fLoadRobot::TfLoadRobot, Difficulty) == 244);

} // namespace fLoadRobot
