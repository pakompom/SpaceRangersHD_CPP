#pragma once
#include "units/GI_MessageLoop.hpp"

namespace GI_MessageLoop {
    inline void TMessageLoopGI_ProcessWindowMessage(GI_MessageLoop::TMessageLoopGI* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        Self->GI_MessageLoop::TMessageLoopGI::ProcessWindowMessage(Message, WParam, LParam);
    }

} // namespace GI_MessageLoop
