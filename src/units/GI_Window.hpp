#pragma once
#include "types/GI_Window.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Window {
    void TWindowGI_Create(TWindowGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TWindowGI_Destroy(TWindowGI* Self);

} // namespace GI_Window
