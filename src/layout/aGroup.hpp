#pragma once
#include "types/aGroup.hpp"

namespace aGroup {
    static_assert(sizeof(void*) != 4 || sizeof(aGroup::TGroupRouteOrder) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroupRouteOrder, Kind) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroupRouteOrder, Target) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroupRouteOrder, Destination) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroupRouteOrder, WaitMode) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroupRouteOrder, WaitUntilTurn) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(aGroup::TGroup) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, CreatedTurn) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, GenerationSeed) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, RandomState) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, Ships) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, Route) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, TargetStar) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(aGroup::TGroup, AssemblyStar) == 28);

} // namespace aGroup
