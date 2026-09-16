#pragma once
#include "types/SE_Planet.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace SE_Planet {
    extern SE_Planet::PPlanetCollisionCircle FirstPlanetCollisionCircle;

    // Links a new entry at the head; only links are initialized.
    PPlanetCollisionCircle AllocatePlanetCollisionCircle();

    void FreePlanetCollisionCircle(PPlanetCollisionCircle Entry);

    void TPlanetSE_Create(TPlanetSE* Self);

    void TPlanetSE_CreateFromGraph(TPlanetSE* Self, const pas::WideString& AGraphKey, WindowsSdk::TPoint UnusedPosition);

} // namespace SE_Planet
