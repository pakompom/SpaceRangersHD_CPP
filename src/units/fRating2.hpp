#pragma once
#include "types/fRating2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fRating2 {
    extern const pas::Array<std::int32_t, 1, 3> CareerHintColumns;

    extern const pas::Array<std::int32_t, 1, 3> DominatorHintColumns;

    std::uint8_t ShowRangerRating(GI_MessageLoop::TMessageLoopGI* Parent);

    void TfRating2_Create(TfRating2* Self);

    void TfRating2_Destroy(TfRating2* Self);

} // namespace fRating2
