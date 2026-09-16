#pragma once
#include "types/fRewards.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fRewards {
    std::uint8_t RunRewards(GI_MessageLoop::TMessageLoopGI* ParentLoop, std::uint8_t ReadOnly);

} // namespace fRewards
