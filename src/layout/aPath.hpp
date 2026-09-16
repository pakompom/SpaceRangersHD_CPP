#pragma once
#include "types/aPath.hpp"

namespace aPath {
    static_assert(sizeof(void*) != 4 || sizeof(aPath::TSPath) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPath, ActiveHead) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPath, ActiveTail) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPath, FreeHead) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPath, FreeTail) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPath, NodeCount) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(aPath::TSPathNode) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPathNode, Prev) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPathNode, Next) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPathNode, Position) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aPath::TSPathNode, Heading) == 16);

} // namespace aPath
