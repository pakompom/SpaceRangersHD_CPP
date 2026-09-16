#pragma once
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace SE_Space {
    // Overwrites Dest without releasing its previous reference.
    void RetainSpaceObject(pas::Var<TObjectSE*> Dest, TObjectSE* Source);

    // Clears Obj; frees its previous value when the decremented reference count is nonpositive.
    void ReleaseSpaceObject(pas::Var<TObjectSE*> Obj);

    void TObjectSE_CreateEmpty(TObjectSE* Self);

    // UnusedPosition is copied but does not initialize Position.
    void TObjectSE_Create(TObjectSE* Self, const pas::WideString& AGraphKey, WindowsSdk::TPoint UnusedPosition);

    void TObjectSE_Destroy(TObjectSE* Self);

    void TSpaceSE_Create(TSpaceSE* Self, GI_Panel::TPanelGI* AMapPanel, GI_MessageLoop::TMessageLoopGI* AScreen);

    // Requires all timers to have been removed.
    void TSpaceSE_Destroy(TSpaceSE* Self);

} // namespace SE_Space
