#pragma once
#include "types/SE_Comet.hpp"

namespace SE_Comet {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Comet::TCometSE) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, TimerInterval) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, MoveTimer) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, ImagePath) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, ReservedImageText) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, ExplosionPath) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, ExplosionFrames) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, TrailPath) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, TrailFrames) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, FirstTrailEntry) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, SavedFrameIndex) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, TrailHistoryCount) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, Animation) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, CompletedExplosion) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, CurrentExplosion) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, SkipMoves) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, Velocity) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, MoveAngle) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, Radius) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, Speed) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, StarAttraction) == 280);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometSE, ObjectAttraction) == 284);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Comet::TCometTrailEntry) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Next) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Velocity) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Animation) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Comet::TCometTrailEntry, Finished) == 28);

} // namespace SE_Comet
