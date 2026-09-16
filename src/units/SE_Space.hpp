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
    void RetainSpaceObject(pas::Var<TObjectSE*> Dest, TObjectSE* Source);

    void ReleaseSpaceObject(pas::Var<TObjectSE*> Obj);

    void TObjectSE_CreateEmpty(TObjectSE* Self);

    void TObjectSE_Create(TObjectSE* Self, const pas::WideString& AGraphKey, WindowsSdk::TPoint UnusedPosition);

    void TObjectSE_Destroy(TObjectSE* Self);

    void TSpaceSE_Create(TSpaceSE* Self, GI_Panel::TPanelGI* AMapPanel, GI_MessageLoop::TMessageLoopGI* AScreen);

    void TSpaceSE_Destroy(TSpaceSE* Self);

} // namespace SE_Space
