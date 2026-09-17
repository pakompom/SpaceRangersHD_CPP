#pragma once
#include "types/GI_Circle.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Circle {
    void TCircleGI_Create(TCircleGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TCircleGI_Destroy(TCircleGI* Self);

    void TCircleGI_LoadFromConfigPath(TCircleGI* Self, const pas::WideString& Path);

} // namespace GI_Circle
