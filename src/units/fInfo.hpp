#pragma once
#include "types/fInfo.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace fInfo {
    extern pas::Array<pas::WideString, 0, 19> InfoNewsAnimationNames;

    // Native selection adds one to each weight.
    extern pas::Array<std::int32_t, 0, 19> InfoNewsAnimationWeights;

    // initializes I=1; increments and repeats until I=7.
    // The indexed FLD therefore reads these six remaining grades, not PirateSlotBonusWeights[-13+I].
    extern pas::Array<float, 0, 6> InfoQualityGrades;

    // Search is already lowercase; positions are one-based, zero means absent.
    std::int32_t FindLowercaseInfoTextFrom(const pas::WideString& Search, const pas::WideString& Text, std::int32_t StartPosition);

    std::int32_t FindLowercaseInfoText(const pas::WideString& Search, const pas::WideString& Text);

    // Search terms must already be lowercase.
    std::uint8_t ItemMatchesInfoSearch(aItem::TItem* Item, pas::WideString Search);

    void TfInfo_Create(TfInfo* Self);

    void TfInfo_Destroy(TfInfo* Self);

} // namespace fInfo
