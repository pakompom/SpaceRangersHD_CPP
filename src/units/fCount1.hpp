#pragma once
#include "types/fCount1.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fCount1 {
    std::uint32_t ShowNumberDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& KindImagePath, const pas::WideString& Caption, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, pas::List* Items, std::int32_t& Value);

    void TfCount1_Create(TfCount1* Self);

    void TfCount1_Destroy(TfCount1* Self);

} // namespace fCount1
