#pragma once
#include "types/GI_MessageBox.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace GI_MessageBox {
    std::uint32_t ShowMessageBoxGI(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& Text, std::uint32_t Options, std::int32_t UnusedOption, std::int32_t OffsetX, std::int32_t OffsetY);

} // namespace GI_MessageBox
