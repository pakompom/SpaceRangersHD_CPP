#pragma once
#include "types/GI_MultiImage.hpp"

namespace GI_MultiImage {
    static_assert(sizeof(void*) != 4 || sizeof(GI_MultiImage::TMultiImageGI) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, FirstUnit) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, LastUnit) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, FirstRow) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, LastRow) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, CellSize) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageGI, Images) == 308);
    static_assert(sizeof(void*) != 4 || sizeof(GI_MultiImage::TMultiImageUnitGI) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, PrevInColumn) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, NextInColumn) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, Column) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, ImageIndex) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, Position) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageUnitGI, UserData) == 36);
    static_assert(sizeof(void*) != 4 || sizeof(GI_MultiImage::TMultiImageColGI) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, First) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, Last) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, Row) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageColGI, Index) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(GI_MultiImage::TMultiImageRowGI) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageRowGI, Prev) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageRowGI, Next) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageRowGI, First) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageRowGI, Last) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageRowGI, Index) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(GI_MultiImage::TMultiImageImageGI) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageImageGI, ImageCache) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(GI_MultiImage::TMultiImageImageGI, Bounds) == 8);

} // namespace GI_MultiImage
