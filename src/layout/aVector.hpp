#pragma once
#include "types/aVector.hpp"

namespace aVector {
    static_assert(sizeof(void*) != 4 || sizeof(aVector::TRectF) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TRectF, Left) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TRectF, Top) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TRectF, Right) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TRectF, Bottom) == 12);
    static_assert(sizeof(void*) != 4 || sizeof(aVector::TPolygon2D) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Previous) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Points) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, GroupId) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Unknown14) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Extent) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, CachedArea) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, AreaValid) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Bounds) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygon2D, Flag39) == 57);
    static_assert(sizeof(void*) != 4 || sizeof(aVector::TPolygonEdge) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygonEdge, First) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aVector::TPolygonEdge, Last) == 8);

} // namespace aVector
