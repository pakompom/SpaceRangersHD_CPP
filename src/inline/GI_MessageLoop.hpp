#pragma once
#include "units/GI_MessageLoop.hpp"

namespace GI_MessageLoop {
    // Dispatches mouse/keyboard messages, deferred UI code and layout-inspector keys. Button coordinates are unsigned words; move and wheel coordinates are signed.
    inline void TMessageLoopGI_ProcessWindowMessage(GI_MessageLoop::TMessageLoopGI* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        Self->GI_MessageLoop::TMessageLoopGI::ProcessWindowMessage(Message, WParam, LParam);
    }

} // namespace GI_MessageLoop
