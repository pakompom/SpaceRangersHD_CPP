#pragma once
#include "types/aGroup.hpp"

namespace aGroup {
    void TGroup_Create(TGroup* Self);

    void TGroup_Destroy(TGroup* Self);

    // May remove and free Self when empty or older than 150 days.
    void TGroup_NextDay(TGroup* Self);

    // Detaches member ships, removes Self from Galaxy.LiberationGroups, and frees Self.
    void TGroup_Disband(TGroup* Self);

} // namespace aGroup
