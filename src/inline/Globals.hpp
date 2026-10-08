#pragma once
#include "units/Globals.hpp"
#include "units/aMyFunction.hpp"

namespace Globals {
    inline std::uint8_t HasShownPlayerTip(std::int32_t Index) {
        return (pas::shr(ShownPlayerTips, Index) & 1) != 0;
    }

    inline std::int32_t SelectSpaceImageTemplate(std::int32_t Kind) {
        return Globals::SelectSpaceImageTemplateFromSeed(Kind, aMyFunction::RandomIntRange(0, 2000000000));
    }

} // namespace Globals
