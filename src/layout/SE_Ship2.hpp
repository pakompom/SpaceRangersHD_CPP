#pragma once
#include "types/SE_Ship2.hpp"

namespace SE_Ship2 {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Ship2::TShip2SE) == 440);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ImageSize) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ImageScale) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ImageOrigin) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ImageCenter) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, Angle) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, Alpha) == 109);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, AlphaLimit) == 110);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, MinimapImagePath) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, AlternateImagePath) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, MinimapImageOrigin) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, StateIntervalMs) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ImagePath) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, ReducedImagePath) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TailOrigins) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TailEmitIntervalMs) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, Image) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, MinimapImage) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, Tails) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TailPrefix) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, WeaponPortCount) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, WeaponPorts) == 280);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, SharedAnimations) == 360);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, FirstAnimation) == 364);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, LastAnimation) == 368);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, FirstReducedAnimation) == 372);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, LastReducedAnimation) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, CurrentAnimation) == 380);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, NextAnimation) == 384);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, DefaultAnimation) == 388);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, DefaultReducedAnimation) == 392);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, CurrentFrameIndex) == 396);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, AnimationTimer) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, StateTimer) == 404);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TotalAnimationWeight) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TotalReducedAnimationWeight) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TailMode) == 416);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, PanelPartnerImage) == 420);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, SmallSize) == 424);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, LargeSize) == 428);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, TargetSizeScale) == 432);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2SE, AngleOverride) == 436);
    static_assert(sizeof(void*) != 4 || sizeof(SE_Ship2::TShip2AnimSE) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, Weight) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, FrameCount) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, Frames) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ship2::TShip2AnimSE, Delays) == 24);

} // namespace SE_Ship2
