#pragma once
#include "types/aGalaxyEvent.hpp"

namespace aGalaxyEvent {
    static_assert(sizeof(void*) != 4 || sizeof(aGalaxyEvent::TGalaxyEvent) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxyEvent::TGalaxyEvent, EventType) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxyEvent::TGalaxyEvent, Turn) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxyEvent::TGalaxyEvent, Data) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(aGalaxyEvent::TGalaxyEvent, TextData) == 16);

} // namespace aGalaxyEvent
