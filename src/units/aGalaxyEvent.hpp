#pragma once
#include "types/aGalaxyEvent.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aGalaxyEvent {
    TGalaxyEvent* AddGalaxyEvent(pas::WideString EventType, aGalaxy::TGalaxy* Galaxy);

    void TGalaxyEvent_Create(TGalaxyEvent* Self, pas::WideString EventType);

    void TGalaxyEvent_Destroy(TGalaxyEvent* Self);

} // namespace aGalaxyEvent
