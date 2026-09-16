#pragma once
#include "types/SE_Angel.hpp"

namespace SE_Angel {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Angel::TAngelSE) == 188);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, TimerInterval) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, MoveTimer) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, ImagePath) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, ImageCount) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, ImagePaths) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, FirstEntry) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, FrameIndex) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, SizeRange) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, TurnTicks) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, Velocity) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, GroupSize) == 152);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, MoveAngle) == 160);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, Speed) == 164);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, EntryCount) == 168);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, Target) == 172);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, TargetDelay) == 180);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelSE, MoveState) == 184);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Angel::TAngelEntry) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Target) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Velocity) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Animation) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, Angle) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, FrameIndex) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, MovingUp) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Angel::TAngelEntry, FrameVariant) == 45);

} // namespace SE_Angel
