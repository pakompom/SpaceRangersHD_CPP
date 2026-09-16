#pragma once
#include "types/fSelectFace.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fSelectFace {
    std::uint8_t RunSelectFaceDialog(GI_MessageLoop::TMessageLoopGI* Parent);

} // namespace fSelectFace
