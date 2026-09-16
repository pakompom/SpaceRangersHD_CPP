#pragma once
#include "types/SE_Process.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace SE_Process {
    // Film-tag factory; copies the eight-byte point and forwards it to the selected constructor. Returns nil for an unknown case-sensitive tag.
    SE_Space::TObjectSE* CreateSpaceObjectByName(const pas::WideString& ClassName, const pas::WideString& GraphKey, WindowsSdk::TPoint UnusedPosition);

    // Returns the film type tag; raises for an unsupported scene class.
    pas::WideString ClassSEtoName(SE_Space::TObjectSE* Obj);

    void TProcessSE_Create(TProcessSE* Self, const pas::WideString& ConfigName);

    void TProcessSE_Destroy(TProcessSE* Self);

} // namespace SE_Process
