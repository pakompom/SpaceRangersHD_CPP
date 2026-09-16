#pragma once
#include "types/fGalaxy2.hpp"

namespace fGalaxy2 {
    static_assert(sizeof(void*) != 4 || sizeof(fGalaxy2::TfGalaxy2) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, MapPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, ViewMode) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, HideBuffer) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, MapPixelBounds) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, GalaxyOrigin) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, GalaxyExtent) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, SelectedJumpStar) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, RouteStars) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, StarLinks) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, StarInfoHideTimer) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpAnimationTimer) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpHintAnimationState) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpLightTick) == 280);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpButton) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpDestinationLabel) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpAnimation) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, JumpLightImages) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, CapturePreviewOnOpen) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, CreateMarkerButton) == 312);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, CreateMarkerImagePath) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, CreateMarkerActiveImagePath) == 320);
    static_assert(sizeof(void*) != 4 || offsetof(fGalaxy2::TfGalaxy2, CreateMarkerMode) == 324);

} // namespace fGalaxy2
