#pragma once
#include "types/SE_Space.hpp"

namespace SE_Space {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Space::TObjectSE) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, ProcessPrev) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, ProcessNext) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, Space) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, GraphKey) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, Size) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, Position) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, DepthExpression) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, SoundLoopPath) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, SoundGroup) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, LoopSound) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, RandomSound) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, RandomSoundGroup) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, NextSoundTime) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TObjectSE, RefCount) == 72);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Space::TSpaceSE) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, FirstObject) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, LastObject) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, FirstTimer) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, LastTimer) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, NextTimerToProcess) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapScale) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MapPanel) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapControl) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapViewportFrame) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, Screen) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapBackground) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapRangeShade) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapRangeCircle) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, StarField) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, StarFieldM) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, SpaceImages) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, StarFieldImages) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, MinimapDragging) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, Process) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, ScrollChangedCallback) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, PathPoints) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, PathPointCount) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceSE, AlphaShift) == 104);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Space::TSpaceTimerSE) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, TicksRemaining) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, RepeatTicks) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, Callback) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Space::TSpaceTimerSE, UserData) == 24);

} // namespace SE_Space
