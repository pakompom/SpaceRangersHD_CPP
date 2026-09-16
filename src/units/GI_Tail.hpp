#pragma once
#include "types/GI_Tail.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Tail {
    // Segments is a Delphi dynamic array, with inactive slots included in SegmentCapacity.
    // SegmentVelocity is displacement per 20 ms movement callback.
    void TTailGI_Create(TTailGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TTailGI_Destroy(TTailGI* Self);

} // namespace GI_Tail
