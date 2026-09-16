#pragma once
#include "types/fPlanetNO.hpp"

namespace fPlanetNO {
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetNO::TProbeTrajectoryPoint) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeTrajectoryPoint, Position) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeTrajectoryPoint, Direction) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetNO::TfPlanetNO) == 31064);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, LoadPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, TrajectoryPointCounts) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, Trajectories) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, SelectedTrajectoryIndex) == 30960);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, SatelliteInventoryPageStart) == 30964);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, SatelliteInventorySlots) == 30968);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ResearchPanelVisible) == 30992);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoWindow) == 30996);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoImage) == 31000);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoNameLabel) == 31004);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoTextLabel) == 31008);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoSizeLabel) == 31012);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoCostLabel) == 31016);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoRaceIcon) == 31020);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ItemInfoHideTimer) == 31024);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, HoveredItem) == 31028);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, SatellitePanelNeedsLayout) == 31032);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, SatelliteMovementTimer) == 31036);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ProbeSignalTimer) == 31040);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ProbeSignalSound) == 31044);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, ProbeSignalCount) == 31048);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, HeldSatellite) == 31052);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, HeldSatelliteOrigin) == 31056);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, HoveringSurfaceLoot) == 31060);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TfPlanetNO, NewSurfaceLootDiscovered) == 31061);
    static_assert(sizeof(void*) != 4 || sizeof(fPlanetNO::TProbeMarkerPixel) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeMarkerPixel, X) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeMarkerPixel, Y) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeMarkerPixel, Pixel) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPlanetNO::TProbeMarkerPixel, Visited) == 12);

} // namespace fPlanetNO
