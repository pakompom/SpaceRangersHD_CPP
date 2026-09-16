#pragma once
#include "types/fCustom.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fCustom {
    extern GI_MessageLoop::TMessageLoopGI* CurrentCustomDialog;

    std::int32_t ShowCustomDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ScreenName);

} // namespace fCustom
