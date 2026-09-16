#pragma once
#include "types/aGalaxyEvent.hpp"

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aGalaxyEvent {
    // Nil Galaxy selects the current galaxy; returns nil if none exists. The galaxy owns the result, dated with its CurrentTurn. Trims the oldest events to retain at most 9999 entries.
    TGalaxyEvent* AddGalaxyEvent(pas::WideString EventType, aGalaxy::TGalaxy* Galaxy);

    void TGalaxyEvent_Create(TGalaxyEvent* Self, pas::WideString EventType);

    void TGalaxyEvent_Destroy(TGalaxyEvent* Self);

} // namespace aGalaxyEvent
