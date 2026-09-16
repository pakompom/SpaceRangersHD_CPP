#pragma once
#include "types/PathClass.hpp"

namespace PathClass {
    static_assert(sizeof(void*) != 4 || sizeof(PathClass::TPath) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, ParameterChanges) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Priority) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, IsAutomatic) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, UnknownFlag) == 17);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, AlwaysShow) == 18);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Available) == 19);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Days) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, DisplayOrder) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Id) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, TraversalLimit) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, TraversalCount) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, FromLocationId) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, ToLocationId) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Caption) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Event) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, ConditionExpression) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(PathClass::TPath, Sequence) == 228);

} // namespace PathClass
