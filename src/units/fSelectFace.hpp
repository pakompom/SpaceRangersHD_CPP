#pragma once
#include "types/fSelectFace.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fSelectFace {
    // Borrows the initialized SelectFaceScreen; returns True only for Run result 1. Caller supplies and reads its selection fields.
    std::uint8_t RunSelectFaceDialog(GI_MessageLoop::TMessageLoopGI* Parent);

} // namespace fSelectFace
