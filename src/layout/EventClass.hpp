#pragma once
#include "types/EventClass.hpp"

namespace EventClass {
    static_assert(sizeof(void*) != 4 || sizeof(EventClass::TEvent) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(EventClass::TEvent, Text) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EventClass::TEvent, Picture) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EventClass::TEvent, Music) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EventClass::TEvent, Sound) == 16);

} // namespace EventClass
