#pragma once
#include "types/fCount2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fCount2 {
    // Native TfCount2 VMT confirms the inferred unit ownership.
    std::uint32_t ShowCountDialogWithFont(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& Description, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, float UnitValue, std::int32_t Available, std::int32_t TotalLimit, std::int32_t& Value, pas::WideString PreviewImagePath, pas::WideString FontName);

    std::uint32_t ShowCountDialog(GI_MessageLoop::TMessageLoopGI* Parent, const pas::WideString& ImagePath, const pas::WideString& Description, std::int32_t Minimum, std::int32_t Maximum, std::int32_t Limit, float UnitValue, std::int32_t Available, std::int32_t TotalLimit, std::int32_t& Value);

    void TfCount2_Create(TfCount2* Self);

    void TfCount2_Destroy(TfCount2* Self);

} // namespace fCount2
