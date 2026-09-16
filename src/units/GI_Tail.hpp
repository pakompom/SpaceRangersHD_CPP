#pragma once
#include "types/GI_Tail.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Tail {
    void TTailGI_Create(TTailGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TTailGI_Destroy(TTailGI* Self);

} // namespace GI_Tail
