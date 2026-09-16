#pragma once
#include "types/aSaveLoad.hpp"

namespace aSaveLoad {
    static_assert(sizeof(void*) != 4 || sizeof(aSaveLoad::TSaver) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, FileName) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, HeaderBuffer) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, PreviewBuffer) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, SecondaryPreviewBuffer) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, GameStateBuffer) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(aSaveLoad::TSaver, FilmBuffer) == 64);

} // namespace aSaveLoad
