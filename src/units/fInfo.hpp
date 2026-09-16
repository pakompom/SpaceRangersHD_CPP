#pragma once
#include "types/fInfo.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace fInfo {
    extern pas::Array<pas::WideString, 0, 19> InfoNewsAnimationNames;

    extern pas::Array<std::int32_t, 0, 19> InfoNewsAnimationWeights;

    extern pas::Array<float, 0, 6> InfoQualityGrades;

    std::int32_t FindLowercaseInfoTextFrom(const pas::WideString& Search, const pas::WideString& Text, std::int32_t StartPosition);

    std::int32_t FindLowercaseInfoText(const pas::WideString& Search, const pas::WideString& Text);

    std::uint8_t ItemMatchesInfoSearch(aItem::TItem* Item, pas::WideString Search);

    void TfInfo_Create(TfInfo* Self);

    void TfInfo_Destroy(TfInfo* Self);

} // namespace fInfo
