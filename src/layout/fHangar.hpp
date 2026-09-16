#pragma once
#include "types/fHangar.hpp"

namespace fHangar {
    static_assert(sizeof(void*) != 4 || sizeof(fHangar::THangarShipSlot) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::THangarShipSlot, AnimationState) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::THangarShipSlot, ShipId) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::THangarShipSlot, ImageBuffer) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::THangarShipSlot, Opacity) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::THangarShipSlot, ImageControl) == 16);
    static_assert(sizeof(void*) != 4 || sizeof(fHangar::TfHangar) == 432);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, StationPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, LoadPanel) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, ShipInfoWindow) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, HoveredShip) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, ShipInfoHideTimer) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, TakeOffPending) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, AmbientAnimationTimer) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, DockedShipsTimer) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, ShipSlots) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fHangar::TfHangar, SelectedShip) == 428);

} // namespace fHangar
