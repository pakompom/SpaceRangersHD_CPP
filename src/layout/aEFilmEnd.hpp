#pragma once
#include "types/aEFilmEnd.hpp"

namespace aEFilmEnd {
    static_assert(sizeof(void*) != 4 || sizeof(aEFilmEnd::TEFilmEnd) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEnd, FirstEntry) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEnd, LastEntry) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(aEFilmEnd::TEFilmEndEntry) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEndEntry, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEndEntry, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEndEntry, SceneObject) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEndEntry, RelatedObject1) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aEFilmEnd::TEFilmEndEntry, RelatedObject2) == 16);

} // namespace aEFilmEnd
