#pragma once
#include "types/Robot.hpp"

namespace Robot {
    static_assert(sizeof(void*) != 4 || sizeof(Robot::TRobotInterfacePrefix) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotInterfacePrefix, Initialize) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotInterfacePrefix, Finalize) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotInterfacePrefix, Support) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotInterfacePrefix, Run) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(Robot::TRobotCallbacks) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, PlaySound) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, CreateSound) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, FreeSound) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, StartSound) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, IsSoundPlaying) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, SetSoundVolume) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, SetSoundPan) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, GetSoundVolume) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, GetSoundPan) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, RenderText) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, FreeText) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, SetProgress) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, PlayMusic) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, ReleaseTextures) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, GetMusicVolume) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotCallbacks, SetMusicVolume) == 60);
    static_assert(sizeof(void*) != 4 || sizeof(Robot::TRobotTextImage) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotTextImage, Buffer) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotTextImage, Pixels) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotTextImage, Pitch) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotTextImage, Width) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotTextImage, Height) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(Robot::TRobotDisplaySettingsPrefix) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Direct3D) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Device) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ShowStencilShadows) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ShowProjShadows) == 9);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, SelectEx) == 10);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, LandTexturesGloss) == 11);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ObjTexturesGloss) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, SoftwareCursor) == 13);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Sky) == 14);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, RobotShadow) == 15);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ColorDepth) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ScreenWidth) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, ScreenHeight) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, RefreshRate) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Brightness) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Contrast) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, FSAASamples) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, Anisotropy) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, MaxDistance) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(Robot::TRobotDisplaySettingsPrefix, VSync) == 52);

} // namespace Robot
