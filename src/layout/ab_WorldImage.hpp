#pragma once
#include "types/ab_WorldImage.hpp"

namespace ab_WorldImage {
    static_assert(sizeof(void*) != 4 || sizeof(ab_WorldImage::TabWorldImage) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Image) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, FrontImagePath) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, BackImagePath) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, FrontDepth) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, BackDepth) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Dirty) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, FrameMode) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, LoopAnimation) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, Finished) == 61);
    static_assert(sizeof(void*) != 4 || offsetof(ab_WorldImage::TabWorldImage, StopAnimation) == 62);

} // namespace ab_WorldImage
