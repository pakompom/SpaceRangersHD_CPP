#pragma once
#include "types/Types.hpp"
#include "types/ab_Space.hpp"

namespace ab_Space {
    extern ab_Space::TabSpace* FirstArcadeSpace;

    extern ab_Space::TabSpace* LastArcadeSpace;

    extern ab_Space::TabSpace* CurrentArcadeSpace;

    extern ab_Space::TabSpace* NextArcadeSpace;

    extern ab_Space::TabSpace* StartArcadeSpace;

    extern ab_Space::TabSpace* EndArcadeSpace;

    extern ab_Space::TabSpace* HoveredArcadeSpace;

    extern ab_Space::PabSpaceLink FirstArcadeSpaceLink;

    extern ab_Space::PabSpaceLink LastArcadeSpaceLink;

    extern std::uint8_t ArcadeKellerEncounter;

    void ab_Space_UpdateApproachDanger();

    void ab_Space_CreateImages();

    void ab_Space_ClearImages();

    void ab_SpaceLink_Invalidate();

    void ab_SpaceLink_BuildGeometry();

    void ab_SpaceLink_Draw();

    void ab_SpaceLink_ClearImages();

    void ab_Space_Clear();

    TabSpace* ab_Space_Add();

    void ab_Space_Delete(TabSpace* Space);

    TabSpace* ab_Space_Find(Types::TPoint GridPosition);

    void ab_Space_RecountLinks();

    void ab_SpaceLink_Clear();

    PabSpaceLink ab_SpaceLink_Add();

    void ab_SpaceLink_Delete(PabSpaceLink Link);

    void ab_SpaceLink_Connect(TabSpace* First, TabSpace* Last);

    PabSpaceLink ab_SpaceLink_Find(TabSpace* First, TabSpace* Last);

    PabSpaceLink ab_SpaceLink_FindExit(TabSpace* First, std::int32_t ExitIndex);

    void ab_Space_Update();

    void TabSpace_Create(TabSpace* Self);

    void TabSpace_Destroy(TabSpace* Self);

} // namespace ab_Space
