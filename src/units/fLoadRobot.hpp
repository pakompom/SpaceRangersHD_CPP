#pragma once
#include "types/fLoadRobot.hpp"

namespace fLoadRobot {
    extern std::uint32_t RobotMapNameColor;

    void TfLoadRobot_Create(TfLoadRobot* Self);

    void TfLoadRobot_Destroy(TfLoadRobot* Self);

} // namespace fLoadRobot
